#ifndef _MODEL_NETWORK_QSWITCH_HPP
#define _MODEL_NETWORK_QSWITCH_HPP

#include <common.hpp>
#include "node.hpp"

namespace model {
namespace network {
	/**
	 * \brief Base class for qswitch node.
	 * 
	 * These nodes having routing functions. We store a list of ports and nodes connected
	 * on these ports in this base class, however we do not alter the adjacency matrix
	 * here, as that is up to the implementing class to decide how ports get routed.
	 * 
	 * We do not override node's create() function here as this is treated as a virtual
	 * class. All children of this class MUST implement a create() function (see node).
	 * 
	 * All ports are zero indexed.
	 */
	class base_node_qswitch : public node {
	 public:
		/**
		 * \brief Constructor takes an id and a port count.
		 */
		base_node_qswitch(const node::id_t id,
				const std::size_t portCount)
				: node(type_t::qswitch, id, portCount) {
			nodesOnPorts = new node*[portCount];
			memset(nodesOnPorts, 0, portCount*sizeof(node*));
		}
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~base_node_qswitch() {
		}
		
		/**
		 * \brief The size of the switch is the number of ports of the switch.
		 * 
		 * This corresponds to the maximum number of connections.
		 */
		inline std::size_t size() {
			return connections().size();
		}
		
		/**
		 * \brief Resize the switch, i.e. add or remove ports.
		 * 
		 * If expanding, the extra ports aren't connected to anything. If shrinking, all
		 * ports beyond the new size are dropped and the adjacency table is shrunk.
		 */
		void resize(const std::size_t newSize);
		
		/**
		 * \brief Connect a node to an empty port without updating the routing table.
		 * 
		 * If there is already a node connected on the port, return false. Otherwise,
		 * return true. This calls update_routing_table().
		 */
		bool connect_node(const std::size_t port, node* const newNode);
		
		/**
		 * \brief Disconnect a node from a port without updating the routing table.
		 * 
		 * If there is no node connected on the port, return false. Otherwise, return
		 * true.
		 */
		bool disconnect_node(const std::size_t port);
		
		/**
		 * \brief Given an ingress node, return an egress node based on the routing table.
		 * 
		 * Returns 0 if the supplied incoming node is not connected to the switch.
		 */
		node* route(const node* const incoming);
		
		/**
		 * \brief Update the routing table based upon the current (implementation defined)
		 * state of the switch.
		 */
		virtual void update_routing_table() = 0;
		
		/**
		 * \brief Set the state of the switch from a string.
		 */
		virtual void set_state_str(const char* const str) = 0;
	
	 protected:
		/**
		 * \brief A list of nodes connected on each port, which the implementing class'
		 * routing table can use.
		 */
		node** nodesOnPorts;
	};
}
}

#endif
