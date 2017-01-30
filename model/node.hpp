#ifndef _MODEL_NODE_HPP
#define _MODEL_NODE_HPP

#include <common.hpp>
#include "adjacency.hpp"
#include "interface.hpp"
#include <map>

namespace model {
	/**
	 * \brief The types of nodes on the network.
	 */
	enum class node_type {
		/**
		 * \brief An endpoint can receive quantum transmissions.
		 */
		endpoint,
		
		/**
		 * \brief A quantum switch can route quantum transmissions from a node to another
		 * node.
		 */
		qswitch,
		
		/**
		 * \brief A null node drops everything sent to it.
		 */
		null
	};
	
	/**
	 * \brief A node on the network.
	 */
	class node {
	 public:
		/**
		 * \brief The node id type.
		 */
		typedef std::uint_fast64_t id_t;
		
		/**
		 * \brief Constructor takes the node_type, a numerical id for the node, the
		 * initial size of the adjacency matrix for the connections the node makes.
		 */
		node(const node_type type,
				const id_t id,
				const std::size_t connectionSize);
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~node();
		
		/**
		 * \brief Initialize a new instance of the node.
		 * 
		 * We must override and implement this in a child.
		 */
		static node* create(const id_t id) {
			UNUSED(id);
			throw std::logic_error(err_msg::_stcimpl);
		}
		
		/**
		 * \brief Return the node id.
		 */
		inline id_t id() const {
			return _id;
		}
		
		/**
		 * \brief Return the type of node.
		 */
		inline node_type type() const {
			return _type;
		}
	
	 protected:
		/**
		 * \brief Return a mutable reference to the adjacency object of this node.
		 */
		inline adjacency<node>& connections() {
			return _connections;
		}
	
	 private:
		/**
		 * \brief The type of the node.
		 */
		node_type _type;
		
		/**
		 * \brief The id of the node.
		 */
		id_t _id;
		
		/**
		 * \brief The connections to other nodes this node has.
		 */
		adjacency<node> _connections;
	};
	
	/**
	 * \brief Create a node of type T with the given id.
	 */
	template<typename T> node* create_node(const node::id_t id) {
		return T::create(id);
	}
	
	/**
	 * \brief A factory for node types that contains a list of all registered node types
	 * by name and a static function pointer to each node's create() that overrides
	 * node::create().
	 */
	struct node_factory {
	 public:
		/**
		 * \brief Node container with name and static function pointer to create().
		 */
		typedef std::map<const char* const, node*(*)(const node::id_t)> node_map_t;
		
		/**
		 * \brief Create a new node type by name with a given id.
		 */
		static node* instantiate(const char* const name, const node::id_t id) {
			for(auto it : node_map()) {
				if(strcmp(it.first, name) == 0) {
					return it.second(id);
				}
			}
			throw std::invalid_argument(err_msg::_tpntfnd);
		}
	
	 protected:
		/**
		 * \brief Return reference to list of registered node types.
		 */
		static node_map_t& node_map() {
			return _node_map;
		}
	
	 private:
		/**
		 * \brief List of registered node types.
		 */
		static node_map_t _node_map;
	};
	
	/**
	 * \brief Register a node within the factory.
	 * 
	 * A child of node should have a private member variable of this type with T as their
	 * own type with their name supplied to the constructor. This automatically registers
	 * the node type within the node_factory.
	 */
	template<typename T> struct node_register : node_factory {
	 public:
		/**
		 * \brief Constructor takes the node type's name and registers it with the
		 * node_facotry.
		 */
		node_register(const char* const name) {
			auto it(node_map().insert({name, &create_node<T>}));
			if(UNLIKELY(!it.second)) {
				throw std::runtime_error(err_msg::_rgstrfl);
			}
		}
	};
}

#endif
