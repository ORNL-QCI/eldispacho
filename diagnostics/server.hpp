#ifndef _DIAGNOSTICS_SERVER_HPP
#define _DIAGNOSTICS_SERVER_HPP

#include <common.hpp>
#include "logger.hpp"
#include <zmq.hpp>

/**
 * \brief The maximum length of time to wait for the push threshold to be satisfied.
 * 
 * \note Milliseconds.
 */
#define DIAGNOSTICS_SERVER_SEND_WAIT_FOR 200

/**
 * \brief The maximum number of items for the push threshold.
 * 
 * \note Number of items.
 */
#define DIAGNOSTICS_SERVER_SEND_THRESHOLD 256

/**
 * \brief The number of times waiting for the push threshold to be satisfied fails before
 * just clearing the buffer.
 */
#define DIAGNOSTICS_SERVER_SEND_FAIL_THRESHOLD 5

namespace diagnostics {
	/**
	 * \brief A zmq server implementation of the logger.
	 * 
	 * This implements the zmq publisher pattern.
	 */
	class server : public logger {
	 public:
		/**
		 * \brief Constructor takes no arguments and create dummy logger that doesn't
		 * really do anything.
		 */
		server();
		
		/**
		 * \brief Constructor takes a shared zmq context object and an endpoint to bind
		 * to.
		 */
		server(::zmq::context_t& context,
				const char* const endpoint);
		
		/**
		 * \brief Destructor.
		 */
		virtual ~server();
	
	 private:
		/**
		 * \brief A reference to the shared zmq context object.
		 */
		zmq::context_t* context;
		
		/**
		 * \brief The endpoint the server is connected to.
		 */
		char endpoint[128];
		
		/**
		 * \brief The implemented work function that executes on a thread.
		 * 
		 * \note Threadsafe.
		 */
		void work();
	};
}

#endif
