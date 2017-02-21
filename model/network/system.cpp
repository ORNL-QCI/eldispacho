#include "system.hpp"
#include "debugger.hpp"
#include "parser.hpp"


namespace model {
namespace network {
	system::system(const char* const topology)
			: node(node::type_t::null, 0) {
		parser::parse_description(topology, *this, _nodeList);
		
		
		debugger debug;
		std::cout << "\033[31m" << "Node List" << "\033[0m" << std::endl;
		debug.print_node_list(std::cout, *this);
		
		std::cout << "\033[31m" << "Node Tree" << "\033[0m" << std::endl;
		debug.print_node_tree(std::cout, static_cast<node&>(*this), SIZE_MAX);
		
		std::cout << "\033[31m" << "Connection Tree" << "\033[0m" << std::endl;
		debug.print_connection_tree(std::cout, static_cast<node&>(*this), SIZE_MAX);
		
		throw std::runtime_error("DONE");
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
