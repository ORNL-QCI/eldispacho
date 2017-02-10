#include "endpoint.hpp"

namespace model {
namespace network {
	const node::registry<null_endpoint> null_endpoint::name("null_endpoint");
	
	null_endpoint::null_endpoint(const node::id_t id, const std::size_t connectionCount)
			: node(type_t::null, id, connectionCount) {
	}
	
	null_endpoint::~null_endpoint() {
	}
	
	const node::registry<client> client::name("client");
	
	client::client(const node::id_t id,
			receiver&& detector,
			const std::size_t connectionCount)
			: base_node_endpoint(id,
			connectionCount,
			std::move(detector)) {
	}
}
}
