#include "adapter.hpp"

namespace simulator {
	std::vector<std::int_fast64_t> get_uniform_integer(client& conn,
			const std::size_t count,
			const std::int_fast64_t lower,
			const std::int_fast64_t upper) {
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("get_uniform_integer"))
				->add<std::size_t>(count)
				->add<std::int_fast64_t>(lower)
				->add<std::int_fast64_t>(upper)));
		
		if(UNLIKELY(rspns.error() != 0)) {
			throw std::runtime_error("Simulator returned error");
		}
	
		return rspns.result<std::vector<std::int_fast64_t> >();
	}
	
	std::vector<double> get_uniform_real(client& conn,
			const std::size_t count,
			const double lower,
			const double upper) {
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("get_uniform_real"))
				->add<std::size_t>(count)
				->add<double>(lower)
				->add<double>(upper)));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<std::vector<double> >();
	}
	
	std::vector<std::uint_fast64_t> get_weighted_integer(client& conn,
			const std::uint_fast64_t count,
			std::vector<double>&& weights) {
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("get_weighted_integer"))
				->add<std::size_t>(count)
				->add<std::vector<double>&&>(std::move(weights))));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<std::vector<std::uint_fast64_t> >();
	}
	
	std::uint_fast64_t create_system(client& conn,
			const char* stateType) {
		#ifdef THROW
		if(UNLIKELY(stateType == nullptr)) {
			throw std::invalid_argument("null pointer");
		}
		#endif
		
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("create_system"))
				->add<const char*, false>(stateType)));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<std::uint_fast64_t>();
	}
	
	bool delete_system(client& conn,
			const std::uint_fast64_t systemId) {
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("delete_system"))
				->add<std::uint_fast64_t>(systemId)));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<bool>();
	}
	
	std::uint_fast64_t create_state(client& conn,
			const std::uint_fast64_t systemId,
			unit&& simUnit) {
		#ifdef THROW
		if(UNLIKELY(simUnit.isNull())) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		#endif
		
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("create_state"))
				->add<std::uint_fast64_t>(systemId)
				->add<const char*, false>(simUnit.dialect())
				->add<const char*, false>(simUnit.description())
				->add<char>(simUnit.line_delimiter())));
			
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<std::uint_fast64_t>();
	}
	
	bool delete_state(client& conn,
			const std::uint_fast64_t systemId,
			const std::uint_fast64_t stateId) {
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("delete_state"))
				->add<std::uint_fast64_t>(systemId)
				->add<std::uint_fast64_t>(stateId)));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<bool>();
	}
	
	bool modify_state(client& conn,
			const std::uint_fast64_t systemId,
			const std::uint_fast64_t stateId,
			unit&& simUnit) {
		#ifdef THROW
		if(UNLIKELY(simUnit.isNull())) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		#endif
		
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("modify_state"))
				->add<std::uint_fast64_t>(systemId)
				->add<std::uint_fast64_t>(stateId)
				->add<const char*, false>(simUnit.dialect())
				->add<const char*, false>(simUnit.description())
				->add<char>(simUnit.line_delimiter())));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<bool>();
	}
	
	std::string measure_state(client& conn,
			const std::uint_fast64_t systemId,
			const std::uint_fast64_t stateId,
			unit&& simUnit) {
		#ifdef THROW
		if(UNLIKELY(simUnit.isNull())) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		#endif
		
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("measure_state"))
				->add<std::uint_fast64_t>(systemId)
				->add<std::uint_fast64_t>(stateId)
				->add<const char*, false>(simUnit.dialect())
				->add<const char*, false>(simUnit.description())
				->add<char>(simUnit.line_delimiter())));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<const char*>();
	}
	
	std::string compute_result(client& conn,
			const std::uint_fast64_t systemId,
			unit&& simUnit) {
		#ifdef THROW
		if(UNLIKELY(simUnit.isNull())) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		#endif
		
		// Our conn.call() takes ownership of the request
		response rspns(conn.call((new request("compute_result"))
				->add<std::uint_fast64_t>(systemId)
				->add<const char*, false>(simUnit.dialect())
				->add<const char*, false>(simUnit.description())
				->add<char>(simUnit.line_delimiter())));
		
		if(UNLIKELY(rspns.error())) {
			throw std::runtime_error("Simulator returned error");
		}
		
		return rspns.result<const char*>();
	}
}
