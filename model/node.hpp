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
		
		/**
		 * \brief Node factory that contains a list of all registered node types
		 * that is used to instantiate a new node.
		 */
		struct factory {
		 public:
			/**
			 * \brief Function pointer type to create static function.
			 */
			using creation_fp_t = node*(*)(const node::id_t);
			
			/**
			 * \brief Node container.
			 */
			using node_map_t = std::map<const char* const, creation_fp_t>;
			
			/**
			 * \brief Create a new node type by name with a given id.
			 */
			static node* instantiate(const char* const name, const id_t id);
			
		 protected:
			/**
			 * \brief Direct instantiation is prohibited.
			 */
			factory() {
			}
			
			/**
			 * \brief Create a node of type T with the given id.
			 */
			template<typename T>
					static inline node* create_node(const node::id_t id) {
				return T::create(id);
			}
			
			/**
			 * \brief Add a node registry to the list.
			 */
			static void add_node(node_map_t::value_type&& o);
		
		 private:
			/**
			 * \brief List of registered node types.
			 */
			static node_map_t _node_map;
		};
		
		/**
		 * \brief Register a node within the factory.
		 * 
		 * A child of node should have a private static const member variable of
		 * this type with T as their own type and with their name supplied to
		 * the constructor.
		 */
		template<typename T> struct registry : private factory {
		 public:
			/**
			 * \brief Constructor takes the node child's name and registers it.
			 */
			registry<T>(const char* const name) {
				factory::add_node({name, &registry::create_node<T>});
			}
		};
	
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
}

#endif
