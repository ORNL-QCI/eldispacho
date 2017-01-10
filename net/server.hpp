#ifndef _NET_SERVER_HPP
#define _NET_SERVER_HPP

#include <common.hpp>
#include "../action.hpp"
#include "../buffer.hpp"
#include "../diagnostics/logger.hpp"
#include "../processor.hpp"
#include "request.hpp"
#include "response.hpp"
#include <arpa/inet.h>
#include <mutex>
#include <zmq.hpp>

/**
 * \brief Maximum number of tx worker threads that can be launched.
 */
#define NET_SERVER_MAX_TX_THREADS 16

/**
 * \brief Maximum number of rx worker threads that can be launched.
 */
#define NET_SERVER_MAX_RX_THREADS 1

#define RPC_SERVER_RX_THREAD_WAIT_FOR 15 // milliseconds
#define RPC_SERVER_RX_RECEIVE_TIMEOUT 100 // milliseconds
#define RPC_SERVER_RX_SEND_TIMEOUT 100 // milliseconds

/**
 * \brief Maximum number of milliseconds to block while waiting to receive a tx message.
 */
#define RPC_SERVER_TX_RECEIVE_TIMEOUT 100

/**
 * \brief Maximum number of milliseconds to block while waiting to send a tx message.
 */
#define RPC_SERVER_TX_SEND_TIMEOUT 100

/**
 * \brief Internally used zmq socket used for communication between frontend listener and
 * backend workers for tx.
 */
#define SERVER_ZMQ_WORKER_LOCATION "inproc://workers"

/**
 * \brief Internally used zmq socket used to signal zmq proxy that binds the frontend
 * listener and the backend workers for tx.
 */
#define SERVER_ZMQ_CONTROL_LOCATION "inproc://control"

namespace net {
	
	/**
	 * \brief An RPC server for interfacing with middleware
	 */
	class rpc_server {
	 public:
		/**
		 * \brief Constructor takes an endpoint to bind to for both rx and tx, along with
		 * a processor and an optional logger.
		 * 
		 * The format should be transport://address where transport is the transport
		 * protocol to be used and address is specific to the transport protocol. For
		 * example a TCP connection on localhost could be tcp://127.0.0.1:12345. When
		 * using network devices under *nix, you may use either the IP address of the
		 * device or the device name, e.g. eth0, lo, etc.
		 */
		rpc_server(::zmq::context_t& context,
				::diagnostics::logger* logger,
				::processor& processor,
				const char* const rxEndpoint,
				const char* const txEndpoint);
		
		/**
		 * \brief Destructor will shutdown server gracefully if it is still up.
		 */
		~rpc_server();
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		rpc_server(const rpc_server&) = delete;
		
		 /**
		 * \brief Move constructor is disabled.
		 */
		rpc_server(rpc_server&&) = delete;
		
		 /**
		 * \brief Assignment operator is disabled.
		 */
		rpc_server& operator=(const rpc_server&) = delete; 
		
		 /**
		 * \brief Move assignment operator is disabled.
		 */
		rpc_server& operator=(rpc_server&&) = delete;
		
		/**
		 * \brief Start the server listening with a particular number of listen threads in
		 * the pool for both rx and tx.
		 * 
		 * \note Threadsafe
		 */
		void listen(const std::size_t rxWorkerCount, const std::size_t txWorkerCount);
		
		/**
		 * \brief Stop the server listening and processing requests.
		 * 
		 * \note Threadsafe
		 */
		void stop();
		
		/**
		 * \brief Return whether the server is running or not.
		 * 
		 * \note Threadsafe
		 */
		inline bool is_running() const {
			return isRunning;
		}
	
	 private:
		/**
		 * \brief Whether or not we are currently listening.
		 */
		bool isRunning;
		
		/**
		 * \brief Whether or not we are signaling to the worker threads to exit.
		 */
		bool doExit;
		
		/**
		 * \brief Mutex to protect startup and shutdown.
		 */
		std::mutex stateChangeMutex;
		
		
		
		
		::processor& processor;
		::diagnostics::logger* logger;
		
				
		/**
		 * \brief The zmq context object.
		 * 
		 * See zmq documentation for more details.
		 */
		zmq::context_t& context;
		
		/**
		 * \brief Maximum number of milliseconds to block with zmq's recv() for tx.
		 * 
		 * \note See the appropriate #define directive in this header file to change this
		 * value. We cannot initialize the value here, since we are required to pass it by
		 * address to zmq. Instead, the value is initialized from that directive within
		 * the server source file.
		 */
		static const int tx_receive_timeout;
		
		/**
		 * \brief Maximum number of milliseconds to block with zmq's send() for tx.
		 * 
		 * \note See the appropriate #define directive in this header file to change this
		 * value. We cannot initialize the value here, since we are required to pass it by
		 * address to zmq. Instead, the value is initialized from that directive within
		 * the server source file.
		 */
		static const int tx_send_timeout;
		
		/**
		 * \brief An array of thread objects where each thread is a tx worker that is
		 * processing requests.
		 */
		std::thread txWorkerThreads[NET_SERVER_MAX_TX_THREADS];
		
		/**
		 * \brief The number of tx worker threads.
		 */
		std::size_t txWorkerThreadCount;
		
		/**
		 * \brief The thread that runs a zmq proxy that connects our front end tx listener
		 * to our backend workers.
		 * 
		 * A zmq control socket is used in stop() that signals this thread to exit.
		 */
		std::thread txProxyThread;
		
		/**
		 * \brief The front end listener that actually binds to our endpoint for tx. All
		 * requests are funnelled through this socket.
		 */
		zmq::socket_t txClients;
		
		/**
		 * \brief An internal socket where requests accepted by the tx frontend socket are
		 * passes to the worker threads that are listening.
		 */
		zmq::socket_t txWorkers;
		
		/**
		 * \brief The zmq control socket that allows us to shutdown our zmq proxy thread
		 * for tx.
		 */
		zmq::socket_t txControl;
		
		/**
		 * \brief The thread that runs a zmq proxy that connects our front end listener to
		 * our backend workers for tx.
		 * 
		 * A zmq control socket is used in stop() that signals this thread to exit.
		 */
		void tx_proxy_work();
		
		/**
		 * \brief Wait for incoming requests for processing.
		 * 
		 * \note Threadsafe
		 */
		void tx_work(const std::size_t workerId);
		
		
		const char* txEndpoint;
		const char* rxEndpoint;
		
		/**
		 * \brief Thread for rx server.
		 */
		std::thread rxWorkerThread;
		
		/**
		 * \brief Wait for data to become available before pushing it to clients for rx.
		 * 
		 * \note Threadsafe
		 */
		void rx_work();
	};
}

#endif
