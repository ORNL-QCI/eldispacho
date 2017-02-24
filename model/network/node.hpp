#ifndef _MODEL_NETWORK_NODE_HPP
#define _MODEL_NETWORK_NODE_HPP

#include <common.hpp>
#include "../interface.hpp"
#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>

// Forward declarations
class processor;

namespace model {
namespace network {
	// Forward declarations
	class system;
	class debugger;
	
	/**
	 * \brief A node on the network.
	 */
	class node {
		// Allow debugger to examine class internals
		friend debugger;
		
		/** \fixme: temporary */
		friend system;
		friend processor;
		
	 public:
		/**
		 * \brief The types of nodes on the network.
		 * 
		 * \note There is a 1-1 correspondence of values with type_t_str().
		 */
		enum class type_t {
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
		 * \brief Return a string version of the given type_t type.
		 */
		static const char* type_t_str(const type_t type) {
			switch(type) {
			 case type_t::endpoint:
				return "endpoint";
			 case type_t::qswitch:
				return "qswitch";
			 case type_t::null:
				return "null";
			}
			throw std::logic_error(err_msg::_unrchcd);
		}
		
		/**
		 * \brief The node id type.
		 */
		using id_t = std::uint_fast64_t;
		
		/**
		 * \brief The default parent for nodes.
		 */
		constexpr static node* const default_parent = nullptr;
		
		/**
		 * \brief \todo
		 */
		using children_list_t = std::vector<node*>;
		
		/** 
		 * \brief \todo
		 */
		using connections_list_t = std::vector<node*>;
		
		/**
		 * \brief Constructor.
		 */
		node(const type_t type,
				const id_t id);
		
		/**
		 * \brief Virtual destructor.
		 * 
		 * \todo Threadsafety.
		 */
		virtual ~node();
		
		/**
		 * \brief Initialize a new instance of the node.
		 * 
		 * \warning We must override and implement this in a child.
		 */
		static node* create(const id_t id) {
			UNUSED(id);
			throw std::logic_error(err_msg::_stcimpl);
		}
		
		/**
		 * \brief Return the node id.
		 */
		inline id_t id() const noexcept {
			return _id;
		}
		
		/**
		 * \brief Return the type of node.
		 */
		inline type_t type() const noexcept {
			return _type;
		}
		
		/**
		 * \brief The number of children this node has.
		 * 
		 * \todo Threadsafety.
		 */
		id_t child_count();
		
		/**
		 * \brief The number of children related to this node.
		 * 
		 * \warning This has no depth limiting.
		 * 
		 * \todo Threadsafety
		 */
		id_t recursive_child_count();
		
		/**
		 * \brief Add a child node to this node.
		 * 
		 * This node will now have responsibility of deallocating the child.
		 * 
		 * \TODO Threadsafety.
		 * 
		 * \returns False if the child argument has the same memory address as
		 * an existing child or has the same id as an existing child. Returns
		 * true otherwise.
		 */
		bool add_child(node& child);
		
		/**
		 * \brief Remove a child node from this node.
		 * 
		 * Deallocates the child.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns False if the node is not a child.
		 */
		bool remove_child(const id_t child);
		
		/**
		 * \brief Determine whether the given node is a child of this node.
		 * 
		 * \todo Threadsafety.
		 */
		bool is_child(const id_t child);
		
		/**
		 * \brief Return the number of connections this node has.
		 * 
		 * \todo Threadsafety.
		 */
		std::uint_fast64_t connection_count();
		
		/**
		 * \brief The number of connections to this node and all children.
		 * 
		 * \note Includes connections between children and other children, not
		 * just the connections to nodes not related to this one.
		 * 
		 * \warning This has no depth limiting.
		 * 
		 * \todo Threadsafety
		 */
		std::uint_fast64_t recursive_connection_count();
		
		/**
		 * \brief Add a connection between another node and this node.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns False if the node is already connected to this node. Returns
		 * true otherwise.
		 */
		bool add_connection(node& other);
		
		/**
		 * \brief Remove a connection between another node and this node.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns False if the other node is not connectioned to this node.
		 * Returns true otherwise.
		 */
		bool remove_connection(const id_t other);
		
		/**
		 * \brief Determine whether the given node is connected to this node.
		 * 
		 * \todo Threadsafety.
		 */
		bool is_connected(const id_t other);
		
		/**
		 * \brief \todo
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns \todo
		 */
		node* parent();
		
		/**
		 * \brief
		 * 
		 * \todo Threadsafety.
		 */
		void set_parent(node* const parent);
		
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
			static node* instantiate(const char* const name,
					const node::id_t id);
			
		 protected:
			/**
			 * \brief Direct instantiation is prohibited.
			 */
			factory() = default;
			
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
		
	 protected:
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
		
		/**
		 * \brief Return a mutable reference to the child list.
		 * 
		 * \warning Does not enforce any threadsafety.
		 */
		inline children_list_t& children() noexcept {
			return _children;
		}
		
		/**
		 * \brief Return a mutable reference to the connections list.
		 * 
		 * \warning Does not enforce any threadsafety.
		 */
		inline connections_list_t& connections() noexcept {
			return _connections;
		}
		
		/**
		 * \brief Get a child node with the given id.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns Null if no node with given id is a child.
		 */
		node* get_child(const id_t id);
		
		/**
		 * \brief Return the name of the implemented node type.
		 */
		virtual const char* name() const noexcept = 0;
		
	 private:
		/**
		 * \brief The default size of the child container.
		 */
		constexpr static const std::size_t children_default_size = 3;
		
		/**
		 * \brief The default size of the connection container.
		 */
		constexpr static const std::size_t connections_default_size = 3;
		
		/**
		 * \brief The type of the node.
		 */
		type_t _type;
		
		/**
		 * \brief The id of the node.
		 */
		id_t _id;
		
		/**
		 * \brief The parent of the node.
		 * 
		 * If this pointer is not null, then the node to which it points shall
		 * contain the current node in its list of children.
		 */
		node* _parent;
		
		/**
		 * \brief List of children this node owns.
		 * 
		 * Upon destruction of this node, all children get destroyed.
		 */
		children_list_t _children;
		
		/**
		 * \brief The connections to other nodes this node has.
		 * 
		 * Upon destruction of this node, all connections get destroyed.
		 */
		connections_list_t _connections;
		
	};
}
}

#endif
