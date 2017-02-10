#include "qswitch.hpp"

namespace model {
namespace network {
	void base_node_qswitch::resize(const std::size_t newSize) {
		if(newSize != size()) {
			auto tempNodesOnPorts = new node*[newSize];
			
			if(newSize > size()) {
				memcpy(tempNodesOnPorts, nodesOnPorts, size()*sizeof(node*));
				memset(&tempNodesOnPorts[size()], 0, (newSize-size())*sizeof(node*));
			}
			
			delete[] nodesOnPorts;
			nodesOnPorts = tempNodesOnPorts;
			
			connections().resize(newSize);
		}
	}
	
	bool base_node_qswitch::connect_node(const std::size_t port, node* newNode) {
		#ifdef THROW
		if(UNLIKELY(port >= size())) {
			throw std::out_of_range(err_msg::_arybnds);
		}
		if(UNLIKELY(newNode == 0)) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		#endif
		
		if(nodesOnPorts[port] == 0) {
			nodesOnPorts[port] = newNode;
			update_routing_table();
			return true;
		}
		return false;
	}
	
	bool base_node_qswitch::disconnect_node(const std::size_t port) {
		#ifdef THROW
		if(UNLIKELY(port >= size())) {
			throw std::out_of_range(err_msg::_arybnds);
		}
		#endif
		
		if(nodesOnPorts[port] != 0) {
			nodesOnPorts[port] = 0;
			return true;
		}
		return false;
	}
	
	node* base_node_qswitch::route(const node* const incoming) {
		#ifdef THROW
		if(UNLIKELY(incoming != 0)) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		#endif
		
		for(std::size_t i = 0; i < connections().size(); i++) {
			if(nodesOnPorts[i] == incoming) {
				return connections().next_neighbor(i);
			}
		}
		
		return 0;
	}
}
}
