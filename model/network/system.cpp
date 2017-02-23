#include "system.hpp"
#include "debugger.hpp"
#include "parser.hpp"


namespace model {
namespace network {
	system::system(const char* const topology)
			: node(node::type_t::null, 0) {
		parser::parse_description(topology, *this, _nodeList);
	}
	
	system::~system() {
	}
	
	node& system::find_node(const node::id_t id) {
		const auto it = _nodeList.find(id);
		if(UNLIKELY(it == _nodeList.end())) std::out_of_range("node not found");
		return *(it->second);
	}
}
}
