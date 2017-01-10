#ifndef _MODEL_TIME_SIMULATION_TIME_HPP
#define _MODEL_TIME_SIMULATION_TIME_HPP

#include <common.hpp>
#include <mutex>

namespace model {
	namespace time {
		/**
		 * \brief Time according to our simulation.
		 */
		struct simulation_time {
		 public:
			/**
			 * \brief Constructor sets the simulation time to t=0.
			 */
			simulation_time() : _now(0) {
			}
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			simulation_time(const simulation_time&) = delete;
			
			 /**
			 * \brief Move constructor is disabled.
			 */
			simulation_time(simulation_time&&) = delete;
			
			 /**
			 * \brief Assignment operator is disabled.
			 */
			simulation_time& operator=(const simulation_time&) = delete; 
			
			 /**
			 * \brief Move assignment operator is disabled.
			 */
			simulation_time& operator=(simulation_time&) = delete;
			
			/**
			 * \brief \todo Documentation
			 */
			inline std::uint_fast64_t now() {
				std::lock_guard<std::mutex> lock(_mutex);
				
				return _now;
			}
		
		 private:
			/**
			 * \brief \todo Documentation
			 */
			std::uint_fast64_t _now;
			
			/**
			 * \brief \todo Documentation
			 */
			std::mutex _mutex;
		};
	}
}

#endif
