#include "client_pool.hpp"

namespace simulator {
	client_pool::client_pool(::zmq::context_t& context)
			: clients{0},
			clientCount(0),
			context(context) {
	}
	
	client_pool::client_pool(client_pool&& old)
			: context(std::move(old.context)) {
		clientCount = old.clientCount;
		old.clientCount = 0;
		memcpy(clients, old.clients, clientCount);
	}
	
	client_pool& client_pool::operator=(client_pool&& old) {
		context = std::move(old.context);
		clientCount = old.clientCount;
		old.clientCount = 0;
		memcpy(clients, old.clients, clientCount);
		
		return *this;
	}
	
	client_pool::~client_pool() {
		for(std::size_t i = 0; i < clientCount; i++) {
			delete clients[i];
		}
	}
	
	void client_pool::add(const char* const endpoint,
			::diagnostics::logger* const logger,
			const int sendTimeout,
			const int receiveTimeout) {
		#ifdef THROW
		if(UNLIKELY(clientCount >= SIMULATOR_CLIENT_POOL_MAX_SIZE)) {
			throw std::bad_alloc();
		}
		#endif
		
		clients[clientCount] = new client(endpoint, context, logger);
		clients[clientCount]->set_timeout(sendTimeout, receiveTimeout);
		clientCount++;
	}
	
	void client_pool::pop() {
		if(clientCount > 0) {
			delete clients[clientCount-1];
			clientCount--;
		}
	}
}
