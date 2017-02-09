#include "circulator_switch.hpp"

namespace model {
	const node::registry<circulator_switch> circulator_switch::register_node("circulator_switch");
	
	circulator_switch::circulator_switch(const node::id_t name,
			const std::size_t portCount)
			: base_node_qswitch(name, portCount),
			_state(chirality::ccw) {
	}
	
	void circulator_switch::set_state(const chirality state) {
		_state = state;
	}
	
	void circulator_switch::update_routing_table() {
		auto& conn = connections();
		auto connCount = size();
		
		switch(_state) {
		 case chirality::cw:
			conn.remove(0, connCount-1);
			conn.add<true>(connCount-1, 0, nodesOnPorts[0]);
			
			for(std::size_t i = 1; i < connCount; i++) {
				conn.remove(i, i-1);
				conn.add<true>(i-1, i, nodesOnPorts[i]);
			}

			break;
		
		 case chirality::ccw:
			conn.remove(connCount-1, 0);
			conn.add<true>(0, connCount-1, nodesOnPorts[connCount-1]);
			
			for(std::size_t i = 1; i < connCount; i++) {
				conn.remove(i-1, i);
				conn.add<true>(i, i-1, nodesOnPorts[i-1]);
			}
			
			break;
		
		 default:
			throw std::runtime_error(err_msg::_undhcse);
		}
	}
	
	void circulator_switch::set_state_str(const char* const str) {
		#ifdef THROW
		if(strcmp(str, "cw") != 0 && strcmp(str, "ccw") != 0) {
			throw std::invalid_argument(err_msg::_tpntfnd);
		}
		#endif
		
		if(strcmp(str, "cw") == 0) {
			set_state(chirality::cw);
		} else if(strcmp(str, "ccw") == 0) {
			set_state(chirality::ccw);
		} else {
			assert(1==2);
		}
		update_routing_table();
			
		/*
		// To avoid problems with alignment, we can't merely cast a pointer. The following
		// get reduces to the same (usually even fewer) assembly instructions than
		// casting the pointer.
		uint32_t str4;
		memcpy(&str4, str, sizeof(str4));
		
		set_state(static_cast<chirality>(str4 & MODEL_CIRCULATOR_SWITCH_STATE_MASK));
		
		update_routing_table();*/
	}
}
