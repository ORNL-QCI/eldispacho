#ifndef _SIMULATOR_ADAPTER_HPP
#define _SIMULATOR_ADAPTER_HPP

#include <common.hpp>
#include "client.hpp"
#include "unit.hpp"
#include <string>
#include <vector>

namespace simulator {
	/** 
	 * \brief Get count many integers between lower and upper with a uniform distribution.
	 */ 
	std::vector<std::int_fast64_t> get_uniform_integer(client& conn,
			const std::size_t count,
			const std::int_fast64_t lower,
			const std::int_fast64_t upper);
	
	/**
	 * \brief Get count many real numbers between lower and upper with a uniform
	 * distribution.
	 */
	std::vector<double> get_uniform_real(client& conn,
			const std::size_t count,
			const double lower,
			const double upper);
	
	/**
	 * \brief Get count many integers between 0 and size(weights) with a distribution
	 * described by weights.
	 */
	std::vector<std::uint_fast64_t> get_weighted_integer(client& conn,
			const std::size_t count,
			std::vector<double>&& weights);
	
	/**
	 * \brief Create a quantum system of a specific state_type and noise_type.
	 */
	std::uint_fast64_t create_system(client& conn,
			const char* const stateType);
	
	/**
	 * \brief Delete a quantum system of a specific id.
	 */
	bool delete_system(client& conn,
			const std::uint_fast64_t systemId);
	
	/**
	 * \brief Create a state.
	 */
	std::uint_fast64_t create_state(client& conn,
			const std::uint_fast64_t systemId,
			unit&& simUnit);
	
	/**
	 * \brief Delete a state.
	 */
	bool delete_state(client& conn,
			const std::uint_fast64_t systemId,
			const std::uint_fast64_t stateId);
	
	/**
	 * \brief Modify a state via a program.
	 */
	bool modify_state(client& conn,
			const std::uint_fast64_t systemId,
			const std::uint_fast64_t stateId,
			unit&& simUnit);
	
	/**
	 * \brief Measure a state described via a program and return a string of results.
	 */
	std::string measure_state(client& conn,
			const std::uint_fast64_t systemId,
			const std::uint_fast64_t stateId,
			unit&& simUnit);
	
	/**
	 * \brief Compute the result of a circuit without state, i.e. the state is never
	 * stored within the system.
	 */
	std::string compute_result(client& conn,
			const std::uint_fast64_t systemId,
			unit&& simUnit);
}

#endif
