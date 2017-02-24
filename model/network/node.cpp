#include "node.hpp"

namespace model {
namespace network {
	node::factory::node_map_t node::factory::_node_map;
	
	node* node::factory::instantiate(const char* const name,
			const node::id_t id) {
		// Easier than manually writing a functor for find? Yeah probably
		for(auto it : _node_map) {
			if(strcmp(name, it.first) == 0) return it.second(id);
		}
		throw std::invalid_argument(err_msg::_tpntfnd);
	}
	
	void node::factory::add_node(node_map_t::value_type&& o) {
		if(!_node_map.insert(o).second) {
			throw std::runtime_error(err_msg::_rgstrfl);
		}
	}
	
	node::node(const type_t type,
			const id_t id)
			: _type(type),
			_id(id),
			_parent(default_parent) {
		_children.reserve(children_default_size);
		_connections.reserve(connections_default_size);
	}
	
	node::~node() {
		for(auto connection : _connections) {
			const auto it = std::find_if(
				connection->connections().begin(),
				connection->connections().end(),
				[=](node* const arg) {
					return (arg->id() == _id);
				}
			);
			if(UNLIKELY(it == connection->connections().end())) {
				// You dun goofed
				throw std::logic_error(err_msg::_undhcse);
			}
			// Remove connection from other node
			connection->connections().erase(it);
		}
		
		for(auto child : _children) {
			delete child;
		}
	}
	
	node::id_t node::child_count() {
		return _children.size();
	}
	
	node::id_t node::recursive_child_count() {
		id_t sum = _children.size();
		for(auto child : _children) {
			sum += child->recursive_child_count();
		}
		return sum;
	}
	
	bool node::add_child(node& child) {
		// Check if the node is already a child
		const auto rc = get_child(child.id());
		if(rc != nullptr) return false;
		_children.push_back(&child);
		return true;
	}
	
	bool node::remove_child(const id_t child) {
		const auto it = std::find_if(
				_children.begin(),
				_children.end(),
				[child](node* const arg) {
					return arg->id() == child;
				}
			);
		if(it == _children.end()) return false;
		delete *it;
		_children.erase(it);
		return true;
	}
	
	bool node::is_child(const id_t child) {
		return (get_child(child) != nullptr);
	}
	
	std::uint_fast64_t node::connection_count() {
		return _connections.size();
	}
	
	std::uint_fast64_t node::recursive_connection_count() {
		std::uint_fast64_t sum = _connections.size();
		for(auto child : _children) {
			sum += child->recursive_connection_count();
		}
		return sum;
	}
	
	bool node::add_connection(node& other) {
		const auto it = std::find_if(
				_connections.begin(),
				_connections.end(),
				[&other](node* const arg) {
					return arg->id() == other.id();
				}
			);
		if(it != _connections.end()) return false;
		// Add connection on this node
		_connections.push_back(&other);
		// Add connection on other node
		other.connections().push_back(this);
		return true;
	}
	
	bool node::remove_connection(const id_t other) {
		const auto it = std::find_if(
				_connections.begin(),
				_connections.end(),
				[other](node* const arg) {
					return (arg->id() == other);
				}
			);
		if(it == _connections.end()) return false;
		const auto otherIt = std::find_if(
				(*it)->connections().begin(),
				(*it)->connections().end(),
				[=](node* const arg) {
					return (arg->id() == _id);
				}
			);
		if(otherIt == (*it)->connections().end()) {
			// You dun goofed
			throw std::logic_error(err_msg::_undhcse);
		}
		// Remove connection from other node
		(*it)->connections().erase(otherIt);
		// Remove connection from this node
		_connections.erase(it);
		return true;
	}
	
	bool node::is_connected(const id_t other) {
		const auto it = std::find_if(
				_connections.begin(),
				_connections.end(),
				[other](node* const arg) {
					return (arg->id() == other);
				}
			);
		return (it != _connections.end());
	}
	
	node* node::get_child(const id_t id) {
		const auto it = std::find_if(
			_children.begin(),
			_children.end(),
			[id](node* const arg) {
				return arg->id() == id;
			}
		);
		if(it == _children.end()) return nullptr;
		return *it;
	}
	
	node* node::parent() {
		return _parent;
	}
	
	void node::set_parent(node* const parent) {
		_parent = parent;
	}
}
}
