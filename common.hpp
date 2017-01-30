#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <common.h>
#include <type_traits>

//#define THROW

/**
 * \brief Convert a scoped enum to its underlying type value.
 * 
 * Taken from Effective Modern C++ (978-1-491-90399-5)
 */
template<typename E>
		constexpr typename std::underlying_type<E>::type
		enum_value(E enumerator) noexcept {
	return static_cast<typename std::underlying_type<E>::type>(enumerator);
}

/**
 * \brief \todo
 */
namespace err_msg {
	const char _nllpntr[] = "null ptr";
	const char _arybnds[] = "array bounds exceeded";
	const char _stcimpl[] = "static unimplemented in child";
	const char _tpntfnd[] = "type not found by name";
	const char _rgstrfl[] = "failed to register child";
	const char _undhcse[] = "unhandled case";
	const char _zrlngth[] = "zero length";
	const char _badtype[] = "bad type";
}

#endif
