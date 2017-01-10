#ifndef _SIMULATOR_CLIENT_HPP
#define _SIMULATOR_CLIENT_HPP

#include <common.hpp>
#include <action.hpp>
#include "../diagnostics/logger.hpp"
#include "response.hpp"
#include "request.hpp"
#include <zmq.hpp>

namespace simulator {
	/**
	 * \brief A client used to connect to a simulation server.
	 */
	class client {
	 public:
		/**
		 * \brief Constructor takes an endpoint to bind to, along with an optional logger.
		 *
		 * The format should be transport://address where transport is the transport
		 * protocol to be used and address is specific to the transport protocol. For
		 * example a TCP connection on localhost could be tcp://127.0.0.1:12345. When
		 * using network devices under *nix, you may use either the IP address of the
		 * device or the device name, e.g. eth0, lo, etc.
		 */
		client(const char* const endpoint,
				::zmq::context_t& context,
				::diagnostics::logger* const logger = 0);
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		client(const client&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		client(client&& old);
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		client& operator=(const client&) = delete; 
		
		/**
		 * \brief Move assignment operator.
		 */
		client& operator=(client&& old);
		
		/**
		 * \brief Destructor will disconnect from the server.
		 */
		~client();
		
		/**
		 * \brief Set the send and receive timeout.
		 */
		void set_timeout(const int sendTimeout, const int receiveTimeout);
		
		/**
		 * \brief Send a request to the server and receive a response.
		 * 
		 * \throws std::runtime_exception if a network problem has prevented us from
		 * sending or receiving the request. A timeout will also throw this exception.
		 */
		response call(request* request);
	
	 private:
		/**
		 * \brief The endpoint we are connecting to.
		 * 
		 * \note We own this memory.
		 */
		char* endpoint;
		
		/**
		 * \brief An optional logger that we use to log the request and response within
		 * call.
		 */
		::diagnostics::logger* logger;
		
		/**
		 * \brief A pointer to a zmq context.
		 */
		std::reference_wrapper<::zmq::context_t> context;
		
		/**
		 * \brief ZMQ socket used to connect to the endpoint.
		 */
		::zmq::socket_t socket;
		
		/**
		 * \brief The timeout when sending data to the endpoint.
		 */
		int sendTimeout;
		
		/**
		 * \brief The timeout when receiving data to the endpoint.
		 */
		int receiveTimeout;
	};
}

#endif
