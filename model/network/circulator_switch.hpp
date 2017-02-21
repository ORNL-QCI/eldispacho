#ifndef _MODEL_NETWORK_CIRCULATOR_SWITCH_HPP
#define _MODEL_NETWORK_CIRCULATOR_SWITCH_HPP

#include <common.hpp>
#include "qswitch.hpp"

namespace model {
namespace network {
	// Forward declarations
	class debugger;
	
	/**
	 * \brief A circulator switch implementation of a qswitch.
	 * 
	 * \fixme Should we keep the option for size other than 3?
	 */
	struct circulator_switch : public qswitch {
		// Allow debugger to examine class internals
		friend debugger;
		
	 public:
		/**
		 * \brief The possible chirality states of the circulator.
		 */
		enum class chirality {
			ccw,
			cw
		};
		
		/**
		 * \brief Constructor.
		 * 
		 * By default the circulator switch initializes to a ccw chirality.
		 */
		circulator_switch(const node::id_t i,
				qswitch::port_id_t portCount = 3);
		
		/**
		 * \brief Initialize a new instance of the class.
		 */
		static node* create(const node::id_t id) {
			return new circulator_switch(id, 3);
		}
		
		/**
		 * \brief Get the chirality of the switch.
		 * 
		 * \todo Threadsafety.
		 */
		chirality state();
		
		/**
		 * \brief Set the chirality of the switch.
		 * 
		 * \todo Threadsafety.
		 */
		void set_state(const chirality state);
		
		/**
		 * \brief Set the state of the switch from a string.
		 */
		void set_state_str(const char* const str);
		
	 protected:
		/**
		 * \brief Get the name of the node implementation.
		 */
		inline const char* name() const noexcept {
			return _name;
		}
		
		/**
		 * \brief Update the routing table for the switch.
		 */
		void update_routing_table();
		
	 private:
		/**
		 * \brief Register node.
		 */
		static const node::registry<circulator_switch> register_node;
		
		/**
		 * \brief Name used to register.
		 */
		static constexpr const char* const _name = "circulator_switch";
		
		/**
		 * \brief The current chirality of the switch.
		 */
		chirality _state;
		
		/**
		 * \brief Route from an ingress port to an egress port.
		 * 
		 * \todo Threadsafety.
		 */
		qswitch::port_id_t _route(const qswitch::port_id_t ingress);
	};
}
}

#endif
