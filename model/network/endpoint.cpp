#include "endpoint.hpp"

namespace model {
namespace network {
	const node::registry<client> client::register_node(_name);
	
	client::client(const node::id_t id,
			receiver&& detector)
			: base_node_endpoint(id, std::move(detector)) {
	}
}
}
