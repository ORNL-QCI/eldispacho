#ifndef _SIMULATOR_RESPONSE_HPP
#define _SIMULATOR_RESPONSE_HPP

#include <common.hpp>
#include <rapidjson/document.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/writer.h>

namespace simulator {
	/**
	 * \brief A JSON response from the simulator.
	 * 
	 * All responses are UTF-8.
	 */
	struct response {
	 public:
		/**
		 * \brief Decoding constructor takes in a JSON cstring that does not have a null
		 * termination.
		 */
		response(const char* const data, const std::size_t size) {
			::rapidjson::MemoryStream ms(data, size);
			_dom.ParseStream(ms);
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		response(const response&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		response(response&& old)
				: _dom(std::move(old._dom)) {
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		response& operator=(const response&) = delete; 
		
		/**
		 * \brief Move assignment operator.
		 */
		response& operator=(response&& old) {
			_dom = std::move(old._dom);
			
			return *this;
		}
		
		/**
		 * \brief Return whether or not an error has occured.
		 */
		inline bool error() const {
			// If we have the error field we probably have it set to true
			if(_dom.HasMember("error") && LIKELY(_dom["error"].GetBool())) {
				return true;
			}
			return false;
		}
		
		/**
		 * \brief Return the number of results.
		 * 
		 * \warning This requires that the result be an array.
		 */
		inline std::size_t result_size() const {
			return _dom["result"].Size();
		}
		
		/**
		 * \brief Return a type T result.
		 * 
		 * \warning You must use a template specialized function.
		 */
		template <typename T> inline T result() const;
	
	 private:
		/**
		 * \brief The rapidjson DOM which contains parsed values.
		 * 
		 * Uses UTF-8 by default.
		 */
		::rapidjson::Document _dom;
	};
	
	/**
	 * \brief Return a cstring result.
	 */
	template <> inline const char*
			response::result<const char*>() const {
		return _dom["result"].GetString();
	}
	
	/**
	 * \brief Return a bool result.
	 */
	template <> inline bool
			response::result<bool>() const {
		return _dom["result"].GetBool();
	}
	
	/**
	 * \brief Return an unsigned long int result.
	 */
	template <> inline std::uint_fast64_t
			response::result<std::uint_fast64_t>() const {
		return _dom["result"].GetUint64();
	}
	
	/**
	 * \brief Return an unsigned long int array result.
	 */
	template <> inline std::vector<std::uint_fast64_t>
			response::result<std::vector<std::uint_fast64_t> >() const {
		auto size = _dom["result"].Size();
		std::vector<std::uint_fast64_t> array(size);
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["result"][i].GetUint64();
		}
		
		return std::move(array);
	}
	
	/**
	 * \brief Return a long int array result
	 */
	template <> inline std::vector<std::int_fast64_t>
			response::result<std::vector<std::int_fast64_t> >() const {
		auto size = _dom["result"].Size();
		std::vector<std::int_fast64_t> array(size);
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["result"][i].GetInt64();
		}
		
		return std::move(array);
	}
	
	/**
	 * \brief Return a double array result.
	 */
	template <> inline std::vector<double>
			response::result<std::vector<double> >() const {
		auto size = _dom["result"].Size();
		std::vector<double> array(size);
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["result"][i].GetDouble();
		}
		
		return std::move(array);
	}
}

#endif
