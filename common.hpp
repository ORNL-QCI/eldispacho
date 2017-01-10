#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <common.h>

//#define THROW

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
