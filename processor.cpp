#include "processor.hpp"

processor::processor(::diagnostics::logger* const logger,
		const char* const simulatorEndpoint,
		model::state& state,
		::zmq::context_t& context)
		: logger(logger),
		st(state), 
		simulatorClientPool(context),
		threadCount(0),
		isRunning(false),
		doExit(false) {
	this->simulatorEndpoint = new char[strlen(simulatorEndpoint) + 1];
	strcpy(this->simulatorEndpoint, simulatorEndpoint);
}

processor::~processor() {
	stop();
	
	if(simulatorEndpoint != 0) {
		delete[] simulatorEndpoint;
	}
}

void processor::start(const std::size_t threadCount) {
	if(UNLIKELY(threadCount > PROCESSOR_MAX_THREADS)) {
		throw std::invalid_argument("thread max exceeded");
	}
	
	lock_t lock(stateChangeMutex);
	
	if(!isRunning) {
		for(std::size_t i = 0; i < threadCount; i++) {
			// Each thread gets its own simulator connection
			// If we don't have enough clients, we add one. If we have too many, we remove
			// one. On the last iteration, we have exactly the number of clients we need.
			if(simulatorClientPool.size() < threadCount) {
				simulatorClientPool.add(simulatorEndpoint, logger);
			} else if(simulatorClientPool.size() > threadCount) {
				simulatorClientPool.pop();
			}
			
			workerThreads[i] = std::thread(&processor::work,
					this,
					i,
					std::ref(simulatorClientPool.get(i)));
		}
		
		// Update our internal state
		isRunning = true;
	}
	
	// Create a system
	/** \todo (move this elsewhere) */
	simulator::create_system(simulatorClientPool.get(0), "chp_state");
}

void processor::stop() {
	lock_t lock(stateChangeMutex);
	
	if(isRunning) {
		doExit = true;
		
		for(std::size_t i = 0; i < threadCount; i++) {
			workerThreads[i].join();
		}
		
		// Update our internal state
		doExit = false;
		isRunning = false;
	}
}

void processor::work(const std::size_t id, ::simulator::client& client) {
	UNUSED(id);
	
	incoming_buffer().set_push_wait_threshold(1);
	std::size_t emptyCount = 0;
	std::size_t emptyCountThreshold = 2;
	
	while(!doExit) {
		if(incomingBuffer.push_wait(PROCESSOR_WORK_WAIT) ||
				++emptyCount >= emptyCountThreshold) {
			emptyCount = 0;
			
			while(incomingBuffer.size() != 0) {
				auto item(incomingBuffer.pop());
				
				switch(item.action()) {
				 case ::action::configure_node:
				 {
					switch(item.from().type()) {
					 case ::model::node_type::endpoint:
					 {
						auto bne = static_cast<model::base_node_endpoint*>(&item.from());
						if(strcmp(item.component(), "receiver") == 0) {
							/** \todo: logging */
							
							auto simUnit = ::simulator::unit(item.parameter<const char*>(0),
									item.parameter<const char*>(1),
									item.parameter<char>(2),
									true);
							
							auto d = model::base_node_endpoint::receiver(1,
										std::move(simUnit));
							
							bne->configure_detector(std::move(d));
							
							break;
						} else if(strcmp(item.component(), "transmitter") == 0) {
							/** \todo: logging */
							
						} else {
							throw std::runtime_error(err_msg::_undhcse);
						}
						
						break; 
					 }
					 case ::model::node_type::qswitch:
					 {
						if(strcmp(item.component(), "routing") == 0) {
							/** \todo: logging */
							
							static_cast<model::base_node_qswitch*>(&item.from())->
									set_state_str(item.parameter<const char*>(1));
						} else {
							throw std::runtime_error(err_msg::_undhcse);
						}
						
						break;
					 }
					 case ::model::node_type::null:
					 {
						// Null endpoints currently have nothing to configure, so drop
						
						break;
					 }
					}
					break;
				 }
				 case ::action::tx:
				 {
					// Traverse the network
					::model::node* incoming = &item.from();
					::model::node* endpointNode = 0;
					::model::node* lastNode = &item.from();
					do {
						::model::node& temp = st.network().find_connecting_node(incoming->id());
						// This prevents bouncing between two nodes
						if(lastNode->id() == temp.id()) {
							endpointNode = incoming;
						} else {
							switch(temp.type()) {
							 case ::model::node_type::qswitch:
							 {
								lastNode = &temp;
								 
								// We've encountered a switch
								auto& switchNode = static_cast<::model::base_node_qswitch&>(temp);
								
								// Hop from the node going in the switch to the node going out
								incoming = switchNode.route(incoming);
								break;
							 }
							 case ::model::node_type::endpoint:
							 {
								endpointNode = &temp;
								break;
							 }
							 case ::model::node_type::null:
							 {
								 // If the nodetype is null, we break out of the processing
								 // loop immediately below this
								 endpointNode = &temp;
								 break;
							 }
							}
						}
					} while(endpointNode == 0);
					
					// If the endpoint node type is null, we drop the transmission
					if(endpointNode->type() == ::model::node_type::null) {
						continue;
					}
					auto receivingClient = static_cast<model::base_node_endpoint*>(endpointNode);
					// If the endpoint node has no configured detector, we drop the transmission
					if(receivingClient->get_detector().simulation_unit().description() == 0) {
						std::cerr << "no detector";
						continue;
					}
					
					// Our simulation circuit description
					/** \todo: this has some problems, especially if incoming and outgoing circuit
					 * is of different dialect or line delimiter
					 */
					std::string circuit = std::string(item.parameter<const char*>(1)) + std::string("\n") +
							std::string(receivingClient->get_detector().simulation_unit().description());
					
					auto measurement(simulator::compute_result(client,
							1,
							simulator::unit(receivingClient->get_detector().simulation_unit().dialect(),
							circuit.c_str(),
							receivingClient->get_detector().simulation_unit().line_delimiter())));
					
					char* ptr;
					std::uint_fast64_t result = strtol(measurement.c_str(), &ptr, 2);
					
					outgoingBuffer.push(::push_message(receivingClient->id(), result, item.tx_timestamp()));
					break;
				 }
				 default:
				 {
					std::cerr << "Unknown action: " << item.action() << std::endl;
				 }
				}
			}
		}
	}
}
