#ifndef _MODEL_TIME_WALL_TIME_HPP
#define _MODEL_TIME_WALL_TIME_HPP

#include <common.hpp>
#include <mutex>

namespace model {
	namespace time {
		/**
		 * \brief Time according to a wall clock.
		 */
		struct wall_time {
		 public:
			/**
			 * \brief Constructor saves the current time as the start time.
			 */
			wall_time() {
				//boost::posix_time::time_duration td(boost::posix_time::microsec_clock::universal_time().time_of_day());
				//_now = td.seconds() + (td.fractional_seconds() << 32);
			}
			
			/**
			 * \brief Copy constructor is disabled.
			 * 
			 * It is disabled implicitly because of the mutex member, so we make this deletion obvious.
			 */
			wall_time(const wall_time&) = delete;
			
			/**
			 * \brief Move constructor is disabled.
			 * 
			 * It is disabled implicitly because of the mutex member, so we make this deletion obvious.
			 */
			wall_time(wall_time&&) = delete;
			
			/**
			 * \brief Assignment operator is disabled.
			 * 
			 * It is disabled implicitly because of the mutex member, so we make this deletion obvious.
			 */
			wall_time& operator=(const wall_time&) = delete; 
			
			/**
			 * \brief Move assignment operator is disabled.
			 * 
			 * It is disabled implicitly because of the mutex member, so we make this deletion obvious.
			 */
			wall_time& operator=(wall_time&) = delete;
			
			/**
			 * \brief \todo Documentation
			 */
			inline std::uint_fast64_t now() {
				std::lock_guard<std::mutex> lock(_mutex);
				
				/** \todo */
				
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
