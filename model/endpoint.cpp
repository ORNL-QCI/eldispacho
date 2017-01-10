#include "endpoint.hpp"

namespace model {
	const node_register<null_endpoint> null_endpoint::name("null_endpoint");
	
	null_endpoint::null_endpoint(const node::id_t id, const std::size_t connectionCount)
			: node(node_type::null, id, connectionCount) {
	}
	
	null_endpoint::~null_endpoint() {
	}
	
	const node_register<client> client::name("client");
	
	client::client(const node::id_t id,
			receiver&& detector,
			const std::size_t connectionCount)
			: base_node_endpoint(id,
			connectionCount,
			std::move(detector)) {
	}
}
