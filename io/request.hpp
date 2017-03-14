#ifndef _IO_REQUEST_HPP
#define _IO_REQUEST_HPP

#include <common.hpp>
#include <rapidjson/document.h>

namespace io {
	/**
	 * \brief A JSON request from the client.
	 * 
	 * All requests are UTF-8.
	 */
	struct request {
	 public:
		/**
		 * \brief Decoding constructor takes in a JSON cstring that is mutable.
		 */
		request(char* const str) {
			#ifdef THROW
			//if(str == 0) {
			//	throw std::invalid_argument(err_msg::_nllpntr);
			//}
			#endif
			
			//WE NEED TO FIX THIS!!
			_dom.Parse(str);
		}
		/**
		 * \brief Copy constructor is disabled.
		 * 
		 * It is disabled implicitly because of the rapidjson member, so we make this deletion obvious.
		 */
		request(const request&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		request(request&& old)
				: _dom(std::move(old._dom)) {
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 * 
		 * It is disabled implicitly because of the rapidjson member, so we make this deletion obvious.
		 */
		request& operator=(const request&) = delete; 
		
		/**
		 * \brief Move assignment operator.
		 */
		request& operator=(request&& old) {
			_dom = std::move(old._dom);
			
			return *this;
		}
		
		/**
		 * \brief Return the method name.
		 */
		inline const char* method() const {
			return _dom["method"].GetString();
		}
		
		/**
		 * \brief Return the number of parameters.
		 */
		inline std::size_t parameter_count() const {
			return _dom["parameters"].Size();
		}
		
		/**
		 * \brief Return a type T parameter by index.
		 * 
		 * \warning You must use a template specialized function.
		 */
		template <typename T> inline T parameter(const std::size_t idx) const;
	
	 private:
		/**
		 * \brief The rapidjson DOM which contains parsed values.
		 * 
		 * Uses UTF-8 by default.
		 */
		::rapidjson::Document _dom;
	};
	
	/**
	 * \brief Return a cstring parameter by index.
	 */
	template <> inline const char*
			request::parameter<const char*>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetString();
	}
	
	/**
	 * \brief Return a cstring array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline const char* const*
			request::parameter<const char* const*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new const char*[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetString();
		}
		
		return array;
	}
	
	/**
	 * \brief Return a bool parameter by index.
	 */
	template <> inline bool
			request::parameter<bool>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetBool();
	}
	
	/**
	 * \brief Return a bool array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline bool*
			request::parameter<bool*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new bool[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetBool();
		}
		
		return array;
	}
	
	/**
	 * \brief Return a char parameter by index.
	 */
	template <> inline char
			request::parameter<char>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetString()[0];
	}
	
	/**
	 * \brief Return an unsigned short parameter by index.
	 * 
	 * \warning Possible loss of precision.
	 */
	template <> inline unsigned short
			request::parameter<unsigned short>(const std::size_t idx) const {
		return static_cast<unsigned short>(_dom["parameters"][idx].GetUint());
	}
	
	/**
	 * \brief Return an unsigned short array parameter by index.
	 * 
	 * \warning Possible loss of precision.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline unsigned short*
			request::parameter<unsigned short*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new unsigned short[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = static_cast<unsigned short>(_dom["parameters"][idx][i].GetUint());
		}
		
		return array;
	}
	
	/**
	 * \brief Return a short parameter by index.
	 * 
	 * \warning Possible loss of precision.
	 */
	template <> inline short
			request::parameter<short>(const std::size_t idx) const {
		return static_cast<short>(_dom["parameters"][idx].GetInt());
	}
	
	/**
	 * \brief Return a short array parameter by index.
	 * 
	 * \warning Possible loss of precision.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline short*
			request::parameter<short*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new short[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = static_cast<short>(_dom["parameters"][idx][i].GetInt());
		}
		
		return array;
	}
	
	/**
	 * \brief Return an unsigned int parameter by index.
	 */
	template <> inline unsigned int
			request::parameter<unsigned int>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetUint();
	}
	
	/**
	 * \brief Return an unsigned int array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline unsigned int*
			request::parameter<unsigned int*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new unsigned int[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetUint();
		}
		
		return array;
	}
	
	/**
	 * \brief Return an int parameter by index.
	 */
	template <> inline int
			request::parameter<int>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetInt();
	}
	
	/**
	 * \brief Return an int array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline int*
			request::parameter<int*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new int[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetInt();
		}
		
		return array;
	}
	
	/**
	 * \brief Return an unsigned long int parameter by index.
	 */
	template <> inline unsigned long int
			request::parameter<unsigned long int>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetUint64();
	}
	
	/**
	 * \brief Return an unsigned long int array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline unsigned long int*
			request::parameter<unsigned long int*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new unsigned long int[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetUint64();
		}
		
		return array;
	}
	
	/**
	 * \brief Return a long int parameter by index.
	 */
	template <> inline long int
			request::parameter<long int>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetInt64();
	}
	
	/**
	 * \brief Return a long int array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline long int*
			request::parameter<long int*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new long int[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetInt64();
		}
		
		return array;
	}
	
	/**
	 * \brief Return a float parameter by index.
	 */
	template <> inline float
			request::parameter<float>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetDouble();
	}
	
	/**
	 * \brief Return a float array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline float*
			request::parameter<float*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new float[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetDouble();
		}
		
		return array;
	}
	
	/**
	 * \brief Return a double parameter by index.
	 */
	template <> inline double
			request::parameter<double>(const std::size_t idx) const {
		return _dom["parameters"][idx].GetDouble();
	}
	
	/**
	 * \brief Return a double array parameter by index.
	 * 
	 * \note Pointer returned must be deallocated.
	 */
	template <> inline double*
			request::parameter<double*>(const std::size_t idx) const {
		auto size = _dom["parameters"][idx].Size();
		auto array = new double[size];
		for(std::size_t i = 0; i < size; i++) {
			array[i] = _dom["parameters"][idx][i].GetDouble();
		}
		
		return array;
	}
}

#endif
