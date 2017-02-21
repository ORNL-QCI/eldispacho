#include "qswitch.hpp"

namespace model {
namespace network {
	qswitch::qswitch(const node::id_t id,
			const port_id_t portCount)
			: node(type_t::qswitch, id),
			_ports(portCount, nullptr) {
		/** todo: resize connection count to match portCount */
		connections().reserve(portCount);
	}
	
	qswitch::~qswitch() {
	}
	
	void qswitch::resize(const port_id_t newSize) {
		if(newSize != _ports.size()) {
			if(newSize < _ports.size()) {
				// Update connections
				for(port_id_t i = newSize; i < _ports.size(); i++) {
					remove_connection(_ports[i]->id());
				}
			}
			
			// Update ports
			_ports.resize(newSize, nullptr);
		}
	}
	
	bool qswitch::connect_port(const port_id_t port, const node::id_t newNode) {
		#ifdef THROW
		if(UNLIKELY(port >= size())) throw std::out_of_range(err_msg::_arybnds);
		#endif
		
		// Cannot connect a port in use
		if(_ports[port] != nullptr) return false;
		const auto it = std::find_if(
				connections().begin(),
				connections().end(),
				[newNode](node* const arg) {
					return (arg->id() == newNode);
				}
			);
		if(it == connections().end()) return false;
		_ports[port] = *it;
		update_routing_table();
		return true;
	}
	
	bool qswitch::disconnect_port(const port_id_t port) {
		#ifdef THROW
		if(UNLIKELY(port >= size())) throw std::out_of_range(err_msg::_arybnds);
		#endif
		
		// Cannot disconnect a port not in use
		if(_ports[port] == nullptr) return false;
		_ports[port] = nullptr;
		return true;
	}
	
	node* qswitch::route(node& incoming) noexcept {
		const port_id_t size = _ports.size();
		for(port_id_t i = 0; i < size; i++) {
			if(_ports[i] == &incoming) return _ports[_route(i)];
		}
		return nullptr;
	}
}
}
