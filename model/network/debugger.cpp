#include "debugger.hpp"
#include "system.hpp"
#include <string>

namespace model {
namespace network {
	void debugger::print_node_list(std::ostream& os, system& system) {
		const std::size_t width = 18;
		
		print_table_header(os,
				std::vector<const char*>{
					"ID",
					"Address",
					"Node Type",
					"Model Type",
					"Child Count",
					"Connection Count"},
				width);
		for(auto i : system._nodeList) {
			print_table_cell(os, i.second->id(), width);
			print_table_cell(os, i.second, width);
			print_table_cell(os, node::type_t_str(i.second->type()), width);
			print_table_cell(os, i.second->name(), width);
			print_table_cell(os, i.second->child_count(), width);
			print_table_cell(os, i.second->connection_count(), width);
			os << std::endl;
		}
	}
	
	void debugger::print_node_tree(std::ostream& os,
			node& root,
			std::size_t depth) {
		const std::size_t width = 40;
		print_table_header(os,
				std::vector<const char*>{
					"ID",
					"Address"},
				width);
		_print_node_tree(os, root, depth, 0, width);
	}
	
	void debugger::_print_node_tree(std::ostream& os,
			node& root,
			std::size_t depth,
			std::size_t level,
			const std::size_t width) {
		/** \todo Add catch for depth exception */
		for(auto& i : root.children()) {
			std::string pad;
			if(level != 0) pad += std::string(level, ' ');
			print_table_cell(os, pad + std::to_string(i->id()), width);
			// Nasty
			std::stringstream addr;
			addr << std::hex << i;
			print_table_cell(os, pad + addr.str(), width);
			os << std::endl;
			
			_print_node_tree(os, *i, depth-1, level+1, width);
		}
	}
	
	void debugger::print_connection_tree(std::ostream& os,
			node& root,
			std::size_t depth) {
		const std::size_t width = 40;
		print_table_header(os,
				std::vector<const char*>{
					"ID",
					"Address"},
				width);
		_print_connection_tree(os, root, depth, 0, width);
	}
	
	void debugger::_print_connection_tree(std::ostream& os,
			node& root,
			std::size_t depth,
			std::size_t level,
			const std::size_t width) {
		/** \fixme This algorithm is terrible!!! */
		/** \todo check for depth exception */
		for(auto& i : root.children()) {
			if(i->connections().size() != 0) {
				std::string pad;
				if(level != 0) pad += std::string(level, ' ');
				print_table_cell(os, pad + std::to_string(i->id()), width);
				// Nasty
				std::stringstream addr;
				addr << std::hex << i;
				print_table_cell(os, pad + addr.str(), width);
				os << std::endl;
				
				pad += " ";
				for(auto j : i->connections()) {
					print_table_cell(os, pad + std::to_string(j->id()), width);
					// Nasty
					std::stringstream addr;
					addr << std::hex << j;
					print_table_cell(os, pad + addr.str(), width);
					os << std::endl;
				}
			}
			_print_connection_tree(os, *i, depth-1, level+1, width);
		}
	}
}
}
