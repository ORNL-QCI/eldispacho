#ifndef _NET_RESPONSE_HPP
#define _NET_RESPONSE_HPP

#include <common.hpp>
#include <rapidjson/document.h>
#include <rapidjson/memorybuffer.h>
#include <rapidjson/writer.h>

namespace net {
	/**
	 * \brief A JSON reponse to a client.
	 * 
	 * All responses are UTF-8.
	 * 
	 * \note When using a constructor that takes control of an array pointer,
	 * this memory is freed in the function rapid_json_array().
	 */
	struct response {
	 private:
		/**
		 * \brief Common initialization code invariant of the result type.
		 */
		inline ::rapidjson::Document _initialize(const bool error) {
			::rapidjson::Document dom;
			dom.SetObject();
			if(error) {
				dom.AddMember("error", error, dom.GetAllocator());
			}
			
			return dom;
		}
		
		/**
		 * \brief Take in an array and output a ::rapidjson::Value array.
		 */
		template <typename T>
				inline ::rapidjson::Value rapid_json_array(const T* const items,
				const std::size_t size,
				::rapidjson::Document::AllocatorType& allocator) {
			assert(items != 0);
			assert(size != 0);
			
			::rapidjson::Value jArray(::rapidjson::kArrayType);
			for(std::size_t i = 0; i < size; i++) {
				jArray.PushBack(items[i], allocator);
			}
			return jArray;
		}
		
		/**
		 * \brief Common write code invariant of the result type.
		 */
		inline void _write(::rapidjson::Document& dom) {
			::rapidjson::Writer<::rapidjson::MemoryBuffer> writer(_jsonBuffer);
			dom.Accept(writer);
		}
	
	 public:
		/**
		 * \brief Construct response with long int result.
		 */
		response(const long int result, const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					::rapidjson::Value(result),
					allocator);
			
			_write(dom);
		}
		
		/**
		 * \brief Construct response with unsigned long int result.
		 */
		response(const unsigned long int result, const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					::rapidjson::Value(result),
					allocator);
			
			_write(dom);
		}
		
		/**
		 * \brief Construct response with bool result.
		 */
		response(const bool result, const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					::rapidjson::Value(result),
					allocator);
			
			_write(dom);
		}
		
		/**
		 * \brief Construct response with cstring result.
		 */
		response(const char* const result, const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					::rapidjson::Value().SetString(::rapidjson::StringRef(result)),
					allocator);
			
			_write(dom);
		}
		
		/**
		 * \brief Construct response with an array of long int result.
		 */
		response(const long int* const result,
				const std::size_t size,
				const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					rapid_json_array<long int>(result, size, allocator),
					allocator);
			
			_write(dom);
		}
		/**
		 * \brief Construct response with an array of unsigned long int result.
		 */
		response(const unsigned long int* const result,
				const std::size_t size,
				const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					rapid_json_array<unsigned long int>(result, size, allocator),
					allocator);
			
			_write(dom);
		}
		
		/**
		 * \brief Construct response with an array of double result.
		 */
		response(const double* const result,
				const std::size_t size,
				const bool error = false) {
			auto dom(_initialize(error));
			auto& allocator(dom.GetAllocator());
			
			dom.AddMember("result",
					rapid_json_array<double>(result, size, allocator),
					allocator);
			
			_write(dom);
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		response(const response&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		response(response&& old)
				: _jsonBuffer(std::move(old._jsonBuffer)) {
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		response& operator=(const response&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		response& operator=(response&& old) {
			_jsonBuffer = std::move(old._jsonBuffer);
			
			return *this;
		}
		
		/**
		 * \brief Return a char array that is a JSON encoding of the object.
		 * 
		 * \note This is not null terminated.
		 */
		inline const char* get_json() const {
			return _jsonBuffer.GetBuffer();
		}
		
		/**
		 * \brief Return a the length of the JSON string.
		 */
		inline std::size_t get_json_size() const {
			return _jsonBuffer.GetSize();
		}
	
	 private:
		/**
		 * \brief The rapidjson MemoryBuffer that holds our JSON string.
		 */
		::rapidjson::MemoryBuffer _jsonBuffer;
	};
}

#endif
