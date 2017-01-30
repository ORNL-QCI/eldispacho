#ifndef _COMMON_H
#define _COMMON_H

#include <cassert>
#include <cstring>
#include <stdexcept>

/**
 * \brief Branch prediction hint for compiler.
 * 
 * These are non-trivially defined for gcc and clang.
 */
#if defined __GNUC__ || defined __clang__
#	define LIKELY(x)	__builtin_expect(x, 1)
#	define UNLIKELY(x)	__builtin_expect(x, 0)
#else
#	define LIKELY(x)	(x)
#	define UNLIKELY(x)	(x)
#endif

/**
 * \brief Used to supress compiler warnings about unused variables.
 */
#define UNUSED(x) (void)(x)

/**
 * \brief Loop unroll hint for the compiler
 * 
 * This has to be put before a function declaration, whichs means all loops within the
 * function will be considered.
 */
#if defined __GNUC__
#	define UNROLL_LOOP __attribute__((optimize("unroll-loops")))
#else
#	define UNROLL_LOOP
#endif

#if defined(__SSE4_2__)
/**
 * \brief Enable SSE4.2 optimization for rapidjson.
 */
//#	define RAPIDJSON_SSE42
#elif defined(__SSE2__)
/**
 * \brief Enable SSE2 optimization for rapidjson.
 */
//#	define RAPIDJSON_SSE2
#endif

/**
 * \brief Get the number of elements in an array.
 */
#define ARRAY_LENGTH(x) (unsigned)(sizeof(x)/sizeof(x[0]))

#endif
