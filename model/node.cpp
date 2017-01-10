#include "node.hpp"

namespace model {
	node_factory::node_map_t node_factory::_node_map;
	
	node::node(const node_type type,
			const id_t id,
			const std::size_t connectionSize)
			: _type(type),
			_id(id),
			_connections(connectionSize) {
	}
	
	node::~node() {
	}
}
