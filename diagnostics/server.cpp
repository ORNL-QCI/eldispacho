#include "server.hpp"

namespace diagnostics {
	server::server()
			: logger(false),
			context(nullptr),
			endpoint{'\0'} {
	}
	
	server::server(::zmq::context_t& context,
			const char* const endpoint)
			: logger(true),
			context(&context),
			endpoint{'\0'} {
		auto eLen = strlen(endpoint) + 1;
		
		if(UNLIKELY(eLen > sizeof(this->endpoint))) {
			throw std::invalid_argument(err_msg::_arybnds);
		}
		
		memcpy(this->endpoint, endpoint, eLen);
	}
	
	server::~server() {
	}
	
	void server::work() {
		#ifdef THROW
		if(UNLIKELY(context == nullptr || endpoint[0] == '\0')) {
			throw std::invalud_argument(err_msg::_nllpntr);
		}
		#endif
		
		zmq::socket_t socket(*context, ZMQ_PUB);
		socket.bind(endpoint);
		
		std::size_t failCount = 0;
		std::size_t failCountThreshold = DIAGNOSTICS_SERVER_SEND_FAIL_THRESHOLD;
		
		set_push_wait_threshold(DIAGNOSTICS_SERVER_SEND_THRESHOLD);
		
		while(!doExit) {
			if(push_wait(DIAGNOSTICS_SERVER_SEND_WAIT_FOR) ||
					++failCount > failCountThreshold) {
				failCount = 0;
				auto localValues = pop_all();
				
				while(!localValues.empty()) {
					// We are only retrieving a pointer here
					message* msg = localValues.front();
					localValues.pop();
					
					// Send header
					// Because the topic string is static, we can ensure its lifetime and
					// thus we need no deallocation.
					socket.send(
							::zmq::message_t(const_cast<char*>(action_str(msg->topic())),
											action_str_size(msg->topic()),
											0,
											0),
							ZMQ_SNDMORE);
					
					// Send data
					msg->generate();
					socket.send(::zmq::message_t(const_cast<char*>(msg->data()),
							msg->size(),
							// This conforms to the requirement imposed by zmq::message_t
							// zero-copy idiom that passes a pointer to the data along
							// with a hint object. Because our data is within the hint
							// object, we just deallocate the hint object, which is our
							// case is a message object. The use of this idiom ensures we
							// do not copy the data of a request in zmq and rather we tell
							// zmq the buffer is safe to use until the message is sent.
							// This function is then called automatically to delete the
							// request object.
							[] (void* data, void* hint) {
								UNUSED(data);
								delete static_cast<message*>(hint);
							},
							msg));
				}
			}
		}
		
		socket.disconnect(endpoint);
	}
}
