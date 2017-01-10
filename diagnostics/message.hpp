#ifndef _DIAGNOSTICS_MESSAGE_HPP
#define _DIAGNOSTICS_MESSAGE_HPP

#include <common.hpp>
#include <action.hpp>
#include <chrono>

namespace diagnostics {	
	/**
	 * \brief \todo
	 */
	struct message {
	 private:
		/**
		 * \brief Alias declaration for the type of clock we use to get time.
		 */
		using clock_t = std::chrono::high_resolution_clock;
		
		/**
		 * \brief Alias declaration for the clock precision type.
		 */
		using clock_prc_t = std::chrono::microseconds;
	
	 public:
		/**
		 * \brief \todo
		 * 
		 * We defer message data generation until we call data(), which means the calling
		 * thread can create this object quickly, but the logging thread can do all the
		 * heavy lifting.
		 */
		message(const action topic,
				const char* const data,
				const std::size_t size)
				: _topic(topic),
				_size(size) {
			using namespace std::chrono;
			_time = duration_cast<clock_prc_t>(clock_t::now().time_since_epoch()).count();
			
			_data = new char[_size];
			memcpy(_data, data, _size);
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		message(const message&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		message(message&& old)
				: _topic(old._topic),
				_size(old._size),
				_time(old._time),
				_data(old._data) {
			old._data = nullptr;
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		message& operator=(const message&) = delete; 
		
		/**
		 * \brief Move assignment operator.
		 */
		message& operator=(message&& old) {
			_topic = old._topic;
			_size = old._size;
			_time = old._time;
			_data = old._data;
			old._data = nullptr;
			
			return *this;
		}
		
		/**
		 * \brief Destructor deallocates memory if required.
		 */
		~message() {
			if(_data) {
				delete[] _data;
			}
		}
		
		/**
		 * \brief
		 */
		inline action topic() const {
			return _topic;
		}
		
		/**
		 * \brief The data string without null terminator.
		 */
		inline const char* data() const {
			return _data;
		}
		
		/**
		 * \brief The number of bytes in data.
		 */
		inline std::size_t size() const {
			return _size;
		}
		
		/**
		 * \brief Replace the message data with an encapsulating JSON object with
		 * additional metadata.
		 */
		void generate() {
			// Not playing with std::string saves us many expensive assembly instructions
			const std::size_t sstr = 45+action_str_size(_topic)+_size;
			char* str = new char[sstr];
			std::size_t i = 10;
			memset(str, '\0', sstr);
			memcpy(str, "{\"topic\":\"", 10);
			sprintf(&str[i], "%s", action_str(_topic));
			i+=action_str_size(_topic);
			str[i] = '"';
			memcpy(&str[++i], ",\"time\":", 9);
			i+=8;
			// This will roll over long after I'm dead
			sprintf(&str[i], "%*lu", 16, _time);
			i+=16;
			memcpy(&str[i], ",\"data\":", 8);
			i+=8;
			memcpy(&str[i], _data, _size);
			i+=_size;
			memcpy(&str[i], "}", 1);
			
			delete[] _data;
			_data = str;
			_size = sstr;
		}
	
	 private:
		/**
		 * \brief Action type enum serves as a topic to file this message as.
		 */
		action _topic;
		
		/**
		 * \brief The number of data bytes.
		 */
		std::size_t _size;
		
		/**
		 * \brief The time the message was created in microseconds since epoch.
		 * 
		 * This must be 64 bits since we require 51 bits to store the full number.
		 */
		std::uint_fast64_t _time;
		
		/**
		 * \brief The message string without a null terminator.
		 */
		char* _data;
	};
}

#endif
