#include "circulator_switch.hpp"

namespace model {
namespace network {
	const node::registry<circulator_switch>
			circulator_switch::register_node(_name);
	
	circulator_switch::circulator_switch(const node::id_t id,
			const qswitch::port_id_t portCount)
			: qswitch(id, portCount),
			_state(chirality::ccw) {
	}
	
	void circulator_switch::set_state(const chirality state) {
			_state = state;
	}
	
	circulator_switch::chirality circulator_switch::state() {
		return _state;
	}
	
	void circulator_switch::update_routing_table() {
	}
	
	void circulator_switch::set_state_str(const char* const str) {
		if(strcmp(str, "cw") == 0) {
			set_state(chirality::cw);
		} else if(strcmp(str, "ccw") == 0) {
			set_state(chirality::ccw);
		} else {
			throw std::invalid_argument(err_msg::_tpntfnd);
		}
		update_routing_table();
	}
	
	qswitch::port_id_t
			circulator_switch::_route(const qswitch::port_id_t ingress) {
		switch(_state) {
		 case chirality::ccw:
			if(ingress == 0) return _ports.size() - 1;
			else return ingress - 1;
		 case chirality::cw:
			if(ingress + 1 == _ports.size()) return 0;
			else return ingress + 1;
		}
		throw std::logic_error(err_msg::_unrchcd);
	}
}
}
