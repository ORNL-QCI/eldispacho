#ifndef _MODEL_CIRCULATOR_SWITCH_HPP
#define _MODEL_CIRCULATOR_SWITCH_HPP

#include <common.hpp>
#include "qswitch.hpp"
#include <boost/detail/endian.hpp>

namespace model {
	/**
	 * \brief A circulator switch takes data on an ingress port and moves it to an egress
	 * port either left or right of the ingress port.
	 */
	struct circulator_switch : public base_node_qswitch {
	 public:
		/**
		 * \brief The type of rotation of the circulator switch.
		 * 
		 * We introduce a bit of weirdness here to speed up the string to chirality
		 * conversion. The numeric values we assign cw and ccw correspond to the result of
		 * a bitwise AND operation on the string representation of the state name with a
		 * value of 0x00FFFFF (in little-endian). So,
		 * cw = 0x__FF7763 and ccw = 0xFF777763, thus:
		 * 0x__FF7763 & 0x00FFFFFF = 0x00FF7763
		 * 0xFF777763 & 0x00FFFFFF = 0xFF777763.
		 * 
		 * Look at the conversion function for more details, including error checking.
		 * This may seem excessive, but it *seriously* reduces the number of instructions,
		 * removes a call to strcmp, and removes branching.
		 */
		#ifdef BOOST_LITTLE_ENDIAN
		#	define MODEL_CIRCULATOR_SWITCH_STATE_MASK 0x00FFFFFF
		#elif defined(BOOST_BIG_ENDIAN)
		#	define MODEL_CIRCULATOR_SWITCH_STATE_MASK 0xFFFFFF00
		#else
		#	error Endianness cannot be detected
		#endif
		enum class chirality {
			/**
			 * \brief Clockwise rotation means nodes are connected in ascending order and
			 * the last node is connected to the first.
			 * 
			 * For e.g. 1 |-> 2, 2 |-> 3, 3 |-> 1.
			 */
			#ifdef BOOST_LITTLE_ENDIAN
			cw = 0x00007763U,
			#elif defined(BOOST_BIG_ENDIAN)
			cw = 0x63770000U,
			#endif
			
			/**
			 * \brief Counterclockwise rotation means nodes are connected in descending
			 * order and the first node is connected to the last.
			 * 
			 * For e.g. 1 |-> 3, 3 |-> 2, 2 |-> 1.
			 */
			#ifdef BOOST_LITTLE_ENDIAN
			ccw = 0x00777763U,
			#elif defined(BOOST_BIG_ENDIAN)
			ccw = 0x63777700U
			#endif
		};
		
		/**
		 * \brief Constructor takes the node name, the number of ports for the switch.
		 * 
		 * You need to call set_state() after you have connected nodes to the switch to
		 * update the routing table. By default the chirality is cw.
		 */
		circulator_switch(const node::id_t name, const std::size_t portCount);
		
		/**
		 * \brief Initialize a new instance of the class.
		 */
		static node* create(const node::id_t id) {
			return new circulator_switch(id, 3);
		}
		
		/**
		 * \brief Return the current chirality of the switch.
		 */
		inline chirality state() const {
			return _state;
		}
		
		/**
		 * \brief Set the chirality of the switch.
		 */
		void set_state(const chirality state);
		
		/**
		 * \brief Update the routing table for the current state of the switch.
		 */
		void update_routing_table();
		
		/**
		 * \brief Set the state of the switch from a string.
		 */
		void set_state_str(const char* const str);
		
	 private:
		/**
		 * \brief Register node.
		 */
		static const node_register<circulator_switch> register_node;
	 
		/**
		 * \brief The current chirality of the switch.
		 */
		chirality _state;
	};
}

#endif
