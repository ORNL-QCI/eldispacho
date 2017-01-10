#include "client.hpp"

namespace simulator {
	client::client(const char* const endpoint,
			::zmq::context_t& context,
			::diagnostics::logger* const logger)
			: logger(logger),
			context(context),
			socket(context, ZMQ_REQ),
			sendTimeout(0),
			receiveTimeout(0) {
		this->endpoint = new char[strlen(endpoint)+1];
		strcpy(this->endpoint, endpoint);
		socket.connect(this->endpoint);
	}
	
	client::client(client&& old)
			: endpoint(old.endpoint),
			logger(old.logger),
			context(std::move(old.context)),
			socket(std::move(old.socket)),
			sendTimeout(old.sendTimeout),
			receiveTimeout(old.receiveTimeout) {
		old.endpoint = 0;
	}
	
	client& client::operator=(client&& old) {
		endpoint = old.endpoint;
		old.endpoint = 0;
		logger = old.logger;
		context = std::move(old.context);
		socket = std::move(old.socket);
		sendTimeout = old.sendTimeout;
		receiveTimeout = old.receiveTimeout;
		
		return *this;
	}
	
	client::~client() {
		if(endpoint != 0) {
			socket.disconnect(endpoint);
			delete[] endpoint;
		}
	}
	
	void client::set_timeout(const int sendTimeout, const int receiveTimeout) {
		this->sendTimeout = sendTimeout;
		this->receiveTimeout = receiveTimeout;
		
		socket.setsockopt(ZMQ_SNDTIMEO, &this->sendTimeout, sizeof(this->sendTimeout));
		socket.setsockopt(ZMQ_RCVTIMEO, &this->receiveTimeout, sizeof(this->receiveTimeout));
	}
	
	response client::call(request* rqst) {
		rqst->generate();
		
		logger->put(::action::simulator_request,
				rqst->get_json_encoded(),
				rqst->get_json_size());
		
		bool networkResult = socket.send(::zmq::message_t((void*)rqst->get_json_encoded(),
				rqst->get_json_size()+1,
				// This conforms to the requirement imposed by zmq::message_t zero-copy
				// idiom that passes a pointer to the data along with a hint object.
				// Because our data is within the hint object, we just deallocate the hint
				// object, which is our case is a request object. The use of the idiom
				// ensures we do not copy the data of a request in zmq and rather we tell
				// zmq the buffer is safe to use until the message is sent. This function
				// is then called automatically to delete the request object.
				[](void* data, void* hint) {
					UNUSED(data);
					delete static_cast<request*>(hint);
				},
				rqst));
		
		if(UNLIKELY(!networkResult)) {
			throw std::runtime_error("network operation failed");
		}
		
		::zmq::message_t rspns;
		networkResult = socket.recv(&rspns);
		
		if(UNLIKELY(!networkResult)) {
			throw std::runtime_error("network operation failed");
		}
		
		logger->put(::action::simulator_response,
				rspns.data(),
				rspns.size());
		
		return response((const char* const)rspns.data(), rspns.size());
	}
}
