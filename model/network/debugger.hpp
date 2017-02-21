#ifndef _MODEL_NETWORK_DEBUGGER_HPP
#define _MODEL_NETWORK_DEBUGGER_HPP

#include <common.hpp>
#include "node.hpp"
#include <iostream>
#include <iomanip>

namespace model {
namespace network {
	/**
	 * \brief A helper that can peek into specific classes and return internal
	 * data useful for debugging.
	 */
	class debugger {
	 public:
		/**
		 * \brief Print a list of nodes in a system.
		 */
		void print_node_list(std::ostream& os,
				system& system);
		
		/**
		 * \brief Print a tree of nodes.
		 */
		void print_node_tree(std::ostream& os,
				node& root,
				std::size_t depth);
		
		/**
		 * \brief Print a tree of connections.
		 */
		void print_connection_tree(std::ostream& os,
				node& root,
				std::size_t depth);
		
	 private:
		/**
		 * \brief Recursive function to print node tree.
		 */
		void _print_node_tree(std::ostream& os,
				node& root,
				std::size_t depth,
				std::size_t level,
				const std::size_t width);
		
		/**
		 * \brief Recursive function to print connection tree.
		 */
		void _print_connection_tree(std::ostream& os,
				node& root,
				std::size_t depth,
				std::size_t level,
				const std::size_t width);
		
		/**
		 * \brief Print a table header with a given width;
		 */
		template<typename T>
				void print_table_header(std::ostream& os,
					std::vector<T>&& vals,
					const std::size_t width) {
			os << "\033[1m" << "\033[4m";
			for(const auto& cell : vals) {
				os << std::left << std::setw(width) << std::setfill(' ') << cell;
			}
			os << "\033[0m" << std::endl;
		}
		
		/**
		 * \brief Print a table cell with a given width.
		 */
		template<typename T>
				void print_table_cell(std::ostream& os,
					T cell,
					const std::size_t width) {
			os << std::left << std::setw(width) << std::setfill(' ') << cell;
		}
	};
}
}

#endif
