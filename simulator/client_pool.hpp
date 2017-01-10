#ifndef _SIMULATOR_CLIENT_POOL_HPP
#define _SIMULATOR_CLIENT_POOL_HPP

#include <common.hpp>
#include "client.hpp"
#include <zmq.hpp>

/**
 * \brief The maximum size of the pool (stored in stack)
 */
#define SIMULATOR_CLIENT_POOL_MAX_SIZE 4

/**
 * \brief The default client timeout when sending.
 * 
 * Setting this too low may result a client will throw an exception because the simulator
 * was too busy to respond within the timeout. Set to -1 to impose no timeout.
 */
#define SIMULATOR_CLIENT_POOL_SENDTO 200

/**
 * \brief The default client timeout when receiving.
 *
 * Setting this to anything other than -1 imposes a timeout which means our client may
 * throw an exception because the simulator hasn't responded before the timeout, which is
 * not normally something we want. Set to -1 to impose no timeout.
 */
#define SIMULATOR_CLIENT_POOL_RECVTO -1

namespace simulator {
	/**
	 * \brief A pool of clients for connecting to a simulator.
	 */
	class client_pool {
	 public:
		/**
		 * \brief Default constructor.
		 */
		client_pool(::zmq::context_t& context);
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		client_pool(const client_pool&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		client_pool(client_pool&& old);
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		client_pool& operator=(const client_pool&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		client_pool& operator=(client_pool&& old);
		
		/**
		 * \brief Destructor clears the clients in the pool.
		 * 
		 * This will call the destructor of each client.
		 */
		UNROLL_LOOP
		~client_pool();
		
		/**
		 * \brief Initialize a new client and add it to the pool.
		 * 
		 * Setting the sendTimeout too low may result a client will throw an exception
		 * because the endpoint was too busy to respond within the timeout.
		 * 
		 * Setting the receiveTimeout to anything other than -1 imposes a timeout which
		 * means our client may throw an exception because the endpoint hasn't responded
		 * before the timeout, which is not normally something we want.
		 */
		void add(const char* const endpoint,
				::diagnostics::logger* const logger = 0,
				const int sendTimeout = SIMULATOR_CLIENT_POOL_SENDTO,
				const int receiveTimeout = SIMULATOR_CLIENT_POOL_RECVTO);
		
		/**
		 * \brief Remove the last client added.
		 * 
		 * If there are no clients in the pool, do nothing.
		 */
		void pop();
		
		/**
		 * \brief Return a client from the given index.
		 */
		inline client& get(const std::size_t index) const {
			#ifdef THROW
			if(UNLIKELY(index >= clientCount)) {
				throw std::invalid_argument("index out of range");
			}
			#endif
			
			return *clients[index];
		}
		
		/**
		 * \brief Return the number of clients.
		 */
		inline std::size_t size() const {
			return clientCount;
		}
		
	 private:
		/**
		 * \brief The list of clients.
		 * 
		 * This is stored on the stack.
		 * 
		 * \note We own the memory of each client.
		 */
		client* clients[SIMULATOR_CLIENT_POOL_MAX_SIZE];
		
		/**
		 * \brief The number of clients stored in the pool.
		 */
		std::size_t clientCount;
		
		/**
		 * \brief The zmq context that is shared among clients.
		 */
		std::reference_wrapper<::zmq::context_t> context;
	};
}

#endif
