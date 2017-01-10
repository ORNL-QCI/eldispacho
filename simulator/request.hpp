#ifndef _SIMULATOR_REQUEST_HPP
#define _SIMULATOR_REQUEST_HPP

#include <common.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace simulator {
	/**
	 * \brief An RPC request for the server.
	 */
	struct request {
	 public:
		/**
		 * \brief Constructor accepts the method name to call.
		 */
		request(const char* const method)
				: _dom(), _allocator(_dom.GetAllocator()) {
			#ifdef THROW
			if(method == nullptr) {
				throw std::invalid_argument("null pointer");
			}
			#endif
			
			_dom.SetObject();
			_dom.AddMember("method",
					::rapidjson::Value(method, _allocator),
					_allocator);
			_dom.AddMember("parameters",
					::rapidjson::Value(::rapidjson::kArrayType),
					_allocator);
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		request(const request&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		request(request&& old)
				: _dom(std::move(old._dom)),
				_allocator(old._allocator) {
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		request& operator=(const request&) = delete; 
		
		/**
		 * \brief Move assignment operator.
		 */
		request& operator=(request&& old) {
			_dom = std::move(old._dom);
			_allocator = old._allocator;
			
			return *this;
		}
		
		/**
		 * \brief Add a parameter to the request parameter array.
		 * 
		 * This returns a pointer to the current object as to implement a fluent
		 * interface.
		 * 
		 * The reallocate template parameter signifies whether or not we are to reallocate
		 * the data when adding it to our request. This is by default true, which doesn't
		 * make a difference when dealing with numeric types. However for cstring this
		 * means we can skip a reallocation of the string.
		 * 
		 * \warning You must use a template specialized function.
		 */
		template <typename T, bool reallocate = true> inline request* add(T data);
		
		/**
		 * \brief Generate the JSON null terminated cstring of the object.
		 */
		inline void generate() {
			::rapidjson::Writer<::rapidjson::StringBuffer> writer(_jsonBuffer);
			_dom.Accept(writer);
		}
		
		/**
		 * \brief Return a JSON null terminated cstring of the object.
		 * 
		 * \note Use get_json_size() for the length of this.
		 */
		inline const char* get_json_encoded(const bool regenerate = false) {
			if(regenerate) {
				generate();
			}
			
			return _jsonBuffer.GetString();
		}
		
		/**
		 * \brief Return a the length of the JSON string.
		 */
		inline std::size_t get_json_size() const {
			return _jsonBuffer.GetSize();
		}
	
	 private:
		/**
		 * \brief The rapidjson dom model we use to store our request until json
		 * generation.
		 */
		::rapidjson::Document _dom;
		
		/**
		 * \brief The allocator for the rapidjson document, used for faster access.
		 */
		std::reference_wrapper<rapidjson::Document::AllocatorType> _allocator;
		
		/**
		 * \brief The rapidjson Buffer that holds our JSON string.
		 */
		::rapidjson::StringBuffer _jsonBuffer;
	};
	
	/**
	 * \brief Add a cstring by reference to the parameter array.
	 */
	template <> inline request*
			request::add<const char*, false>(const char* data) {
		#ifdef THROW
		if(data == 0) {
			throw std::invalid_argument("null pointer");
		}
		#endif
		
		_dom["parameters"].PushBack(
				::rapidjson::Value().SetString(::rapidjson::StringRef(data)),
				_allocator);
		
		return this;
	}
	
	/**
	 * \brief Add a char to the parameter array.
	 */
	template <> inline request*
			request::add<char>(char data) {
		_dom["parameters"].PushBack(data, _allocator);
		
		return this;
	}
	
	/**
	 * \brief Add an unsigned long int to the parameter array.
	 */
	template <> inline request*
			request::add<unsigned long int>(unsigned long int data) {
		_dom["parameters"].PushBack(data, _allocator);
		
		return this;
	}
	
	/**
	 * \brief Add an long int to the parameter array.
	 */
	template <> inline request*
			request::add<long int>(long int data) {
		_dom["parameters"].PushBack(data, _allocator);
		
		return this;
	}
	
	/**
	 * \brief Add a double to the parameter array.
	 */
	template <> inline request*
			request::add<double>(double data) {
		_dom["parameters"].PushBack(data, _allocator);
		
		return this;
	}
	
	/**
	 * \brief Add a vector of doubles to the parameter array.
	 */
	template <> inline request*
			request::add<std::vector<double>&&>(std::vector<double>&& data) {
		::rapidjson::Value values(::rapidjson::kArrayType);
		for(auto i : data) {
			values.PushBack(i, _allocator);
		}
		_dom["parameters"].PushBack(values, _allocator);
		
		return this;
	}
}

#endif
