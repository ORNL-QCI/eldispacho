#ifndef _MODEL_STATE_HPP
#define _MODEL_STATE_HPP

#include <common.hpp>
#include "time/simulation_time.hpp"
#include "time/wall_time.hpp"
#include "time/functions.hpp"
#include "network.hpp"

namespace model {
	/**
	 * \brief The internal state of the processor.
	 */
	struct state {
	 public:
		/**
		 * \brief Constructor.
		 */
		state(const char* const topology)
				: _network(topology) {
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 * 
		 * It is disabled implicitly, so we make this deletion obvious.
		 */
		state(const state&) = delete;
		
		/**
		 * \brief Move constructor is disabled.
		 * 
		 * It is disabled implicitly, so we make this deletion obvious.
		 */
		state(state&&) = delete;
		
		/**
		 * \brief Assignment operator is disabled.
		 * 
		 * It is disabled implicitly, so we make this deletion obvious.
		 */
		state& operator=(const state&) = delete; 
		
		/**
		 * \brief Move assignment operator is disabled.
		 * 
		 * It is disabled implicitly, so we make this deletion obvious.
		 */
		state& operator=(state&) = delete;
		
		/**
		 * \brief Return wall time object.
		 * 
		 * \returns A mutable reference to the wall time state object.
		 */
		inline time::wall_time& wall_time() {
			return _wallTime;
		}
		
		/**
		 * \brief Return simulation time object.
		 * 
		 * \returns A mutable reference to the simulation time state object.
		 */
		inline time::simulation_time& sim_time() {
			return _simTime;
		}
		
		/**
		 * \brief Return network object.
		 * 
		 * \returns A mutable reference to the network state object.
		 */
		inline ::model::network& network() {
			return _network;
		}
	
	 private:
		/**
		 * \brief The wall time state object.
		 */
		time::wall_time _wallTime;
		
		/**
		 * \brief The simulation time state object.
		 */
		time::simulation_time _simTime;
		
		/**
		 * \brief The network state object.
		 */
		::model::network _network;
	};
}
#endif
