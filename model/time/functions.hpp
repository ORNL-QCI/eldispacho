#ifndef _MODEL_TIME_FUNCTIONS_HPP
#define _MODEL_TIME_FUNCTIONS_HPP

#include <common.hpp>

namespace model {
	namespace time {
		/**
		 * \brief Pack a decimal reprentation of seconds and fractional seconds.
		 * 
		 * The result is a packed 64 bit unsigned integer where the lower 32
		 * bits represent seconds and the upper 32 bits represent fractional
		 * seconds.
		 * 
		 * Thus, both seconds and fractional seconds have 32 bit resolution,
		 * which corresponds to a maximum of 136.2 years and a mininum of
		 * 233 picoseconds, respectively.
		 */
		inline std::uint_fast64_t pack(const double input) {
			// Add the number of seconds to the output
			std::uint_fast64_t output = (std::uint_fast64_t)input;
			/** \todo */
			/*
			// for each digit after decimal, add number of represented fractional seconds of each digit
			// to the upper 32 bits
			// Assume microseconds, the lowest precision we need is something like
			// 0.000001, or n in 10^n. 
			
			// Only save decimal
			double working(input-output);
			
			for(std::size_t i = 0; i < 10; i++) {
				// Only care about the single digit we get after cast
				// we can relate to the number of bit shifts maybe?
				(std::uint_fast64_t)working;
				working *= 10;
			}*/
			
			return output;
		}
	}
}

#endif
