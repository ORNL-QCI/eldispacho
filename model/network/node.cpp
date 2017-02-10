#include "node.hpp"

namespace model {
namespace network {
	node::factory::node_map_t node::factory::_node_map;
	
	node* node::factory::instantiate(const char* const name, const id_t id) {
		for(auto it : _node_map) {
			if(strcmp(name, it.first) == 0) {
				return it.second(id);
			}
		}
		throw std::invalid_argument(err_msg::_tpntfnd);
	}
	
	void node::factory::add_node(node_map_t::value_type&& o) {
		if(UNLIKELY(!_node_map.insert(o).second)) {
			throw std::runtime_error(err_msg::_rgstrfl);
		}
	}
	
	node::node(const type_t type,
			const id_t id,
			const std::size_t connectionSize)
			: _type(type),
			_id(id),
			_connections(connectionSize) {
	}
	
	node::~node() {
	}
}
}
