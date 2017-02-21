#ifndef _MODEL_NETWORK_SYSTEM_HPP
#define _MODEL_NETWORK_SYSTEM_HPP

#include <common.hpp>
#include "node.hpp"
#include <map>

namespace model {
namespace network {
	/**
	 * \brief \todo
	 */
	class system : public node{
		friend debugger;
		
	 public:
		/**
		 * \brief \todo
		 */
		using node_list_t = std::map<node::id_t, node*>;
		
		/**
		 * \brief Constructor
		 */
		system(const char* const topology);
		
		/**
		 * \brief Destructor.
		 */
		~system();
		
		/**
		 * \brief Return a pointer to the node with the given id.
		 * 
		 * \throws std::out_of_range if node with the given id is not found.
		 */
		node& find_node(const node::id_t id);
		
		/**
		 * \brief Return the name of the implementation type.
		 */
		virtual inline const char* name() const noexcept {
			return "system";
		}
	
	 private:
		/**
		 * \brief A sorted node list for efficient lookup when preprocessing.
		 * 
		 * \todo This may not be the fastest container to use. Although the
		 * complexity may be log_2(n), testing with sorted vectors and sets
		 * seems to suggest that maps are most costly to interact with in
		 * general.
		 */
		node_list_t _nodeList;
	};
}
}

#endif
