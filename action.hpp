#ifndef _ACTION_HPP
#define _ACTION_HPP

#include <common.hpp>

/**
 * \brief The actions the dispatcher can perform.
 * 
 * \warning Order must correspond to _actions[].
 */
enum class action {
	configure_detector,
	tx,
	configure_qswitch,
	configure_node,
	
	// Internal
	rx,
	simulator_request,
	simulator_response,
	
	_COUNT
};

/**
 * \brief Metadata for an action enum value.
 */
struct _action {
	/**
	 * \brief The null terminated string.
	 */
	const char* str;
	
	/**
	 * \brief The size of str excluding the null terminator.
	 */
	const std::size_t size;
};

#define DECLARE_ACTION(str) {str, sizeof(str)-1}

/**
 * \brief The corresponding metadata for each action enum value.
 * 
 * \warning Order must correspond to action.
 */
constexpr _action _actions[7] = {
		DECLARE_ACTION("configure_detector"),
		DECLARE_ACTION("tx"),
		DECLARE_ACTION("configure_qswitch"),
		DECLARE_ACTION("configure_node"),
		DECLARE_ACTION("rx"),
		DECLARE_ACTION("simulator_request"),
		DECLARE_ACTION("simulator_response"),
};

static_assert(ARRAY_LENGTH(_actions) == enum_value<action>(action::_COUNT),
		"Missing element in _actions");

/**
 * \brief Return the corresponding string for a given action in _actions.
 * 
 * For GCC -O1 and above, a call to this will be replaced with an immediate pointer.
 */
constexpr const char* action_str(const action A) {
	#ifdef THROW
	if(UNLIKELY(A == action::_COUNT)) {
		throw std::invalid_argument(err_msg::_arybnds);
	}
	#endif
	
	return _actions[enum_value<action>(A)].str;
}

/**
 * \brief Return the corresponding size for a given action in _actions.
 * 
 * For GCC -O1 and above, a call to this will be replaced with an immediate value.
 */
constexpr std::size_t action_str_size(const action A) {
	#ifdef THROW
	if(UNLIKELY(A == action::_COUNT)) {
		throw std::invalid_argument(err_msg::_arybnds);
	}
	#endif
	
	return _actions[enum_value<action>(A)].size;
}

/**
 * \brief Element comparison function for _action_str_max_impl().
 */
template <class T> constexpr T& _action_str_max(T& a, T& b) {
	return ((a.size < b.size) ? b : a);
}

/**
 * \brief Function to calculate maximum action string length.
 */
template <class T> constexpr T& _action_str_max_impl(T* begin, T* end) {
	return ((begin + 1 == end)
		? *begin
		: _action_str_max(*begin, _action_str_max_impl(begin + 1, end)));
}

/**
 * \brief The maximum size of an action string in _actions.
 */
constexpr std::size_t _action_str_max_size = _action_str_max_impl(_actions,
		&_actions[sizeof(_actions)/sizeof(_actions[0]) - 1]).size;

#endif
