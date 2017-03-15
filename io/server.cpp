#include "server.hpp"

namespace io {
	const int rpc_server::tx_receive_timeout = RPC_SERVER_TX_RECEIVE_TIMEOUT;
	const int rpc_server::tx_send_timeout = RPC_SERVER_TX_SEND_TIMEOUT;
	
	rpc_server::rpc_server(::zmq::context_t& context,
			::diagnostics::logger* logger,
			::processor& processor,
			const char* const rxEndpoint,
			const char* const txEndpoint)
			: isRunning(false),
			doExit(false),
			processor(processor),
			logger(logger),
			context(context),
			txClients(context, ZMQ_ROUTER),
			txWorkers(context, ZMQ_DEALER),
			txControl(context, ZMQ_PUB),
			txEndpoint(txEndpoint),
			rxEndpoint(rxEndpoint) {
	}
	
	rpc_server::~rpc_server() {
		stop();
	}
	
	void rpc_server::listen(const std::size_t rxWorkerCount, 
			const std::size_t txWorkerCount) {
		if(UNLIKELY(rxWorkerCount > IO_SERVER_MAX_RX_THREADS ||
				txWorkerCount > IO_SERVER_MAX_TX_THREADS)) {
			throw std::runtime_error(err_msg::_arybnds);
		}
		
		std::lock_guard<std::mutex> lock(stateChangeMutex);
		
		if(!isRunning) {
			/*
			 * Tx Server
			 */
			// Front end where we listen for client requests
			txClients.bind(txEndpoint);
			// Internal address worker threads will retrieve work from
			txWorkers.bind(SERVER_ZMQ_WORKER_LOCATION);
			// Control socket used to send commands to the steerable proxy,
			// the device that connects the front end and internal worker address
			txControl.connect(SERVER_ZMQ_CONTROL_LOCATION);
			
			// Launch worker thread pool
			for(std::size_t i = 0; i < txWorkerCount; i++) {
				txWorkerThreads[i] = std::thread(&rpc_server::tx_work, this, i);
			}
			
			txWorkerThreadCount = txWorkerCount;
			
			// Connect work threads to client threads via a so called proxy.
			// This has a queue that worker threads then consume in a an
			// (FCFS) manner.
			txProxyThread = std::thread(&rpc_server::tx_proxy_work, this);
			
			/*
			 * Rx Server
			 */
			
			rxWorkerThread = std::thread(&rpc_server::rx_work, this);
			
			isRunning = true;
		}
	}
	
	void rpc_server::stop() {
		std::lock_guard<std::mutex> lock(stateChangeMutex);
		
		if(isRunning) {
			doExit = true;
			
			for(auto& worker : txWorkerThreads) {
				worker.join();
			}
			
			// Shutdown our routing proxy
			/** \todo I believe this also closes txWorkers socket, but need to check */
			txControl.send("TERMINATE", 9);
			txControl.unbind(SERVER_ZMQ_CONTROL_LOCATION);
			txProxyThread.join();
			
			rxWorkerThread.join();
			
			//delete rxEndpoint;
			//rxEndpoint = 0;
			
			doExit = false;
			isRunning = false;
		}
	}
	
	void rpc_server::rx_work() {
		// Context is threadsafe
		zmq::socket_t socket(context, ZMQ_PUB);
		socket.bind(rxEndpoint);
		
		processor.outgoing_buffer().set_push_wait_threshold(1);
		std::size_t emptyCount = 0;
		std::size_t emptyCountThreshold = 2;
		
		while(!doExit) {
			if(processor.outgoing_buffer().push_wait(RPC_SERVER_RX_THREAD_WAIT_FOR) ||
					++emptyCount >= emptyCountThreshold) {
				emptyCount = 0;
				
				// This is a safe call, if the outgoing_buffer is empty, our localValues
				// will have a size equal to 0, which is caught by the loop below
				auto localValues = processor.outgoing_buffer().pop_all();
				
				while(localValues.size() != 0) {
					// This is to get around an oversight in the C++11 standard where our
					// std::queue cannot get an item from the queue by move, only by
					// reference. This is safe because this we hold the only copy of this
					// queue so we know nothing will change between front() and pop().
					auto item = new push_message(std::move(const_cast<push_message&>(localValues.front())));
					localValues.pop();
					
					logger->put(::action::rx,
							item->json_data(),
							item->get_json_size());
					
					
					// The topic is a numeric so copying is not a big deal
					socket.send(item->topic_ch(),
							sizeof(push_message::topic_t),
							ZMQ_SNDMORE);
					
					socket.send(::zmq::message_t((void*)item->json_data(),
							item->get_json_size()+1,
							// This conforms to the requirement imposed by zmq::message_t
							// zero-copy idiom that passes a pointer to the data along
							// with a hint object. Because our data is within the hint
							// object, we just deallocate the hint object, which is our
							// case is a request object. The use of the idiom ensures we
							// do not copy the data of a request in zmq and rather we tell
							// zmq the buffer is safe to use until the message is sent.
							// This function is then called automatically to delete the
							// request object.
							[] (void* data, void* hint) {
								UNUSED(data);
								delete static_cast<push_message*>(hint);
							},
							item));
				}
			}
		}
		
		socket.disconnect(rxEndpoint);
	}
	
	void rpc_server::tx_work(const std::size_t txWorkerId) {
		UNUSED(txWorkerId);
		
		// Context is threadsafe
		zmq::socket_t socket(context, ZMQ_REP);
		socket.connect(SERVER_ZMQ_WORKER_LOCATION);
		socket.setsockopt(ZMQ_RCVTIMEO, &tx_receive_timeout, sizeof(tx_receive_timeout));
		socket.setsockopt(ZMQ_SNDTIMEO, &tx_send_timeout, sizeof(tx_send_timeout));
		
		while(!doExit) {
			response* reply = 0;
			zmq::message_t requestMsg;
			// Wait to receive request from client
			if(socket.recv(&requestMsg)) {
				// Process
				request request(static_cast<char* const>(requestMsg.data()));
				
				if(strcmp(request.method(), action_str(action::configure_node)) == 0) {
					// Avoid null terminator
					logger->put(::action::configure_node,
							requestMsg.data(),
							requestMsg.size()-1);
					
					auto newItem = processor.preprocess(action::configure_node,
							ntohl(request.parameter<unsigned int>(0)),
							request.parameter<const char*>(1),
							request.parameter<const char*>(2),
							request.parameter<const char*>(3),
							request.parameter<char>(4));
					
					processor.incoming_buffer().push(std::move(newItem));
					
					reply = new response(true);
				} else if(strcmp(request.method(), action_str(action::tx)) == 0) {
					// Transmit data
					// Avoid null terminator
					logger->put(::action::tx,
							requestMsg.data(), 
							requestMsg.size()-1);
					
					processor.incoming_buffer().push(processor.preprocess(action::tx,
							ntohl(request.parameter<unsigned int>(0)),
							"",
							request.parameter<const char*>(1),
							request.parameter<const char*>(2),
							request.parameter<const char*>(3)[0]));
					
					reply = new response(true);
				} else if(strcmp(request.method(), action_str(action::configure_qswitch)) == 0) {
					// Avoid null terminator
					logger->put(::action::configure_qswitch,
							requestMsg.data(),
							requestMsg.size()-1);
					
					//\todo: fix this up 
					processor.incoming_buffer().push(processor.preprocess(action::configure_node,
							ntohl(request.parameter<unsigned int>(0)),
							"routing",
							request.parameter<const char*>(1),
							request.parameter<const char*>(2),
							0));
					reply = new response(true);
				} else {
					// Invalid method request
					throw std::runtime_error(err_msg::_undhcse);
				}
				
				// Send Response
				socket.send(::zmq::message_t((void*)reply->get_json(),
						reply->get_json_size(),
						[] (void* data, void* hint) {
							UNUSED(data);
							delete static_cast<response*>(hint);
						},
						reply));
			}
		}
		
		socket.disconnect(SERVER_ZMQ_WORKER_LOCATION);
	}
	
	void rpc_server::tx_proxy_work() {
		// We listen for control signals for our proxy on this socket
		::zmq::socket_t controlSub = ::zmq::socket_t(context, ZMQ_SUB);
		controlSub.setsockopt(ZMQ_SUBSCRIBE, "", 0);
		controlSub.connect(SERVER_ZMQ_CONTROL_LOCATION);
		
		// We block until the control socket receives a TERMINATE command
		zmq::proxy_steerable((void*)txClients, (void*)txWorkers, 0, (void*)controlSub);
		
		controlSub.disconnect(SERVER_ZMQ_CONTROL_LOCATION);
	}
}
