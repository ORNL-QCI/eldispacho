#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <common.hpp>

namespace utility {
	/**
	 * \brief Compile time rounding up of a number to the nearest multiple.
	 */
	template <typename T>
			constexpr inline T round_to_multiple(const T number, const T multiple) {
		static_assert(std::is_arithmetic<T>::value, "T must be of arithmetic type.");
		
		return (((number + multiple - 1) / multiple) * multiple);
	}
	
	/**
	 * \brief Round a number up to the nearest multiple at run-time.
	 * 
	 * Compiler optimizations will greatly reduce the number of instructions here.
	 */
	template <typename T, T multiple>
			inline T round_to_multiple(const T number) {
		return (((number + multiple - 1) / multiple) * multiple);
	}
	
	/**
	 * \brief Determine if two arrays of different types overlap in memory.
	 */
	template <typename T, typename U>
			inline bool memory_overlap(const T* const a,
				const std::size_t aSize,
				const U* const b,
				const std::size_t bSize) {
		return ((a <= b && b <= &a[aSize-1]) || (b <= a && a <= &b[bSize-1]));
	}
	
	/**
	 * \brief Determine if two arrays of the same type overlap in memory.
	 */
	template <typename T>
			inline bool memory_overlap(const T* const a,
				const std::size_t aSize,
				const T* const b,
				const std::size_t bSize) {
		return ((a <= b && b <= &a[aSize-1]) || (b <= a && a <= &b[bSize-1]));
	}
	
	/**
	 * \brief Change a block of unsigned ASCII digits to integral type T.
	 * 
	 * \throws If exception throwing is enabled, throws std::invalid_argument if
	 * the size of the string is given to be zero or if any character other than
	 * ASCII 0 through 9 is encountered.
	 */
	template <typename T = std::size_t>
			inline T stuif(const char* const data,
			const std::size_t size) {
		static_assert(std::is_integral<T>::value, "Type T must be integral");
		
		#ifdef ELDISPACHO_THROW
		if(UNLIKELY(size == 0)) {
			throw std::invalid_argument(err_msg::_zrlngth);
		}
		if(UNLIKELY(!('0' <= data[0] && data[0] <= '9'))) {
			std::invalid_argument(err_msg::_badvals);
		}
		#endif
		T result = data[0] - '0';
		
		for(std::size_t i = 1; i < size; i++) {
			#ifdef ELDISPACHO_THROW
			if(UNLIKELY(!('0' <= data[i] && data[i] <= '9'))) {
				std::invalid_argument(err_msg::_badvals);
			}
			#endif
			result *= 10;
			result += data[i] - '0';
		}
		
		return result;
	}
}

#endif
