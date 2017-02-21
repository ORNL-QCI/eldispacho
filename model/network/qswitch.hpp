#ifndef _MODEL_NETWORK_QSWITCH_HPP
#define _MODEL_NETWORK_QSWITCH_HPP

#include <common.hpp>
#include "node.hpp"

namespace model {
namespace network {
	// Forward declarations
	class debugger;
	
	/**
	 * \brief A quantum switch which establishes manages connected nodes with
	 * some specific mechanism.
	 * 
	 * \note All ports are zero indexed.
	 */
	class qswitch : public node {
		// Allow debugger to examine class internals
		friend debugger;
		
	 public:
		/**
		 * \brief The port id type.
		 */
		using port_id_t = std::uint_fast64_t;
		
		/**
		 * \brief Constructor.
		 */
		qswitch(const node::id_t id, const port_id_t portCount);
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~qswitch();
		
		/**
		 * \brief The size of the switch is the number of ports of the switch.
		 * 
		 * This corresponds to the maximum number of connections and may or may
		 * not be equal to the current number of connections.
		 * 
		 * \todo Threadsafety.
		 */
		inline port_id_t port_count() {
			return _ports.size();
		}
		
		/**
		 * \brief Resize the switch.
		 * 
		 * If expanding, the extra ports aren't connected to anything. If
		 * shrinking, all ports beyond the new size are dropped. The connection
		 * list is updated accordingly.
		 * 
		 * \todo threadsafety
		 */
		void resize(const port_id_t newSize);
		
		/**
		 * \brief Connect a node already in the connection list to a specific
		 * port.
		 * 
		 * If the port connection cannot be made successfully, the call is
		 * idempotent. If it does complete successfully, additional calls with
		 * identical parameters will fail.
		 * 
		 * \note Requires other node already to be connected.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns True if node has been successfully connected to the port,
		 * false otherwise.
		 */
		bool connect_port(const port_id_t port, const node::id_t newNode);
		
		/**
		 * \brief Disconnect a node already in the connection list from a
		 * specific port.
		 * 
		 * Does not remove corresponding node from the connection list.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns True if node has been successfully disconnected from the
		 * port, false otherwise.
		 */
		bool disconnect_port(const port_id_t port);
		
		/**
		 * \brief Find the node connected to the incoming node through the
		 * routing mechanism.
		 * 
		 * \todo Threadsafety.
		 * 
		 * \returns Null if the route cannot be made. Otherwise return a pointer
		 * to the connected node,
		 */
		node* route(node& incoming) noexcept;
		
		/**
		 * \brief Set the state of the switch from a string.
		 */
		virtual void set_state_str(const char* const str) = 0;
		
	 protected:
		/**
		 * \brief A list of nodes connected on each port.
		 * 
		 * \note Null entries correspond to an unused port.
		 */
		std::vector<node*> _ports;
		
		/**
		 * \brief Update the internal mechanism used for routing between ports.
		 */
		virtual void update_routing_table() = 0;
		
	 private:
		/**
		 * \brief Route from an ingress port to an egress port.
		 */
		virtual port_id_t _route(const port_id_t ingress) = 0;
	};
}
}

#endif
