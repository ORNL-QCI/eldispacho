#ifndef _MODEL_CHANNEL_HPP
#define _MODEL_CHANNEL_HPP

#include <common.hpp>
#include "node.hpp"

namespace model {
	/**
	 * \brief A channel is an object that consists of two node endpoints, along with some
	 * parameters.
	 */
	struct channel {
	 public:
		/**
		 * \brief Constructor takes two nodes that represent the endpoints.
		 */
		channel(node& endpointA,
				node& endpointB)
				: _endpointA(endpointA),
				_endpointB(endpointB) {
		}
		
		/**
		 * \brief Return a mutable reference to the second endpoint.
		 */
		inline node& endpoint_a() {
			return _endpointA;
		}
		
		/**
		 * \brief Return a mutable reference to the second endpoint.
		 */
		inline node& endpoint_b() {
			return _endpointB;
		}
		
	 private:
		node& _endpointA;
		node& _endpointB;
	};
}

#endif
