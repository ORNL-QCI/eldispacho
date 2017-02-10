#ifndef _BUFFER_HPP
#define _BUFFER_HPP

#include <common.hpp>
#include "action.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace model { namespace network { class node; } }

/**
 * \brief A message to be pushed over a zmq publisher.
 */
struct push_message {
 public:
	typedef std::uint_fast64_t topic_t;
	
	/**
	 * \brief Constructor takes data to encode into json.
	 */
	push_message(const std::uint_fast64_t topic, const std::uint_fast64_t result, const std::uint_fast64_t timestamp)
			:_topic(topic), _timestamp(timestamp) {
		::rapidjson::Document dom;
		dom.SetObject();
		dom.AddMember("result", ::rapidjson::Value(result), dom.GetAllocator());
		::rapidjson::Writer<::rapidjson::StringBuffer> writer(_jsonBuffer);
		dom.Accept(writer);
	}
	
	/**
	 * \brief Copy constructor is disabled.
	 */
	push_message(const push_message&) = delete;
	
	/**
	 * \brief Move constructor.
	 */
	push_message(push_message&& old)
			: _jsonBuffer(std::move(old._jsonBuffer)) {
		_topic = old._topic;
		_timestamp = old._timestamp;
	}
	
	/**
	 * \brief Assignment operator is disabled.
	 */
	push_message& operator=(const push_message&) = delete;
	
	/**
	 * \brief Move assignment operator.
	 */
	push_message& operator=(push_message&& old) {
		_jsonBuffer = std::move(old._jsonBuffer);
		_topic = old._topic;
		_timestamp = old._timestamp;
		
		return *this;
	}
	
	/**
	 * \brief Topic of message.
	 */
	inline std::uint_fast64_t topic() const {
		return _topic;
	}
	
	/**
	 * \brief Topic of message.
	 *
	 * Length is sizeof(topic()).
	 */
	inline const char* topic_ch() const {
		return (const char* const)&_topic;
	}
	
	/**
	 * \brief Body of the message encoded in json.
	 */
	inline const char* json_data() const {
		return _jsonBuffer.GetString();
	}
	
	/**
	 * \brief Return a the length of the JSON string.
	 */
	inline std::size_t get_json_size() const {
		return _jsonBuffer.GetSize();
	}
	
	/**
	 * \brief The timestamp of when the message was measured.
	 */
	inline std::uint_fast64_t timestamp() const {
		return _timestamp;
	}

 private:
	std::uint_fast64_t _topic;
	std::uint_fast64_t _timestamp;
	/**
	 * \brief The rapidjson MemoryBuffer that holds our JSON string.
	 */
	::rapidjson::StringBuffer _jsonBuffer;
};

namespace model {
	// Forward declaration
	class node;
	class channel;
}

/**
 * \brief A message to be processed.
 */
struct interpreted_request {
 public:
	/**
	 * \brief Constructor.
	 */
	interpreted_request(::action type,
			::model::network::node& from,
			const char* component,
			const char* dialect, const char* circuit, const char lineDelimiter,
			const std::uint_fast64_t txTimestamp)
			: _type(type), _from(from), _component(component),
			 _txTimestamp(txTimestamp) {
		_parameters.push_back(std::string(dialect));
		_parameters.push_back(std::string(circuit));
		_parameters.push_back(std::string(1, lineDelimiter));
	}
	
	/**
	 * \brief \todo Documentation.
	 */
	inline ::action action() const {
		return _type;
	}
	
	/**
	 * \brief \todo Documentation.
	 */
	inline ::model::network::node& from() const {
		return _from;
	}
	
	const char* component() const {
		return _component.c_str();
	}
	
	/**
	 * \brief Return the timestamp of the message.
	 */
	std::uint_fast64_t tx_timestamp() const {
		return _txTimestamp;
	}
	
	/**
	 * \brief \todo
	 */
	template <typename T> inline T parameter(const std::size_t index) const;

 private:
	::action _type;
	::model::network::node& _from;
	std::string _component;
	std::vector<std::string> _parameters;
	std::uint_fast64_t _txTimestamp;
};

/**
 * \brief \todo
 */
template<>
		inline char interpreted_request::parameter<char>(const std::size_t index) const {
	return _parameters[index].c_str()[0];
}

/**
 * \brief \todo
 */
template <>
		inline const char* interpreted_request::parameter<const char*>(const std::size_t index) const {
	return _parameters[index].c_str();
}

/**
 * \brief A FIFO buffer to contain items for single producer single consumer.
 */
 
 /** \todo Single producer single consumer may not extend to threaded model */
 
template <typename T> class buffer {
 private:
	typedef std::unique_lock<std::mutex> lock_t;

 public:
	/**
	 * \brief Constructor.
	 */
	buffer()
			: pushWaitNew(0),
			pushWaitThreshold(0) {
	}
	
	/**
	 * \brief Copy constructor is disabled.
	 */
	buffer(const buffer&) = delete;
	
	/**
	 * \brief Move constructor is disabled.
	 */
	buffer(buffer&&) = delete;
	
	/**
	 * \brief Assignment operator is disabled.
	 */
	buffer& operator=(const buffer&) = delete; 
	
	/**
	 * \brief Move assignment operator is disabled.
	 */
	buffer& operator=(buffer&) = delete;
	
	/**
	 * \brief Set the threshold when a call to push_wait_for will be successful.
	 * 
	 * \note Threadsafe
	 */
	inline void set_push_wait_threshold(const std::size_t threshold) {
		lock_t lock(queueMutex);
		
		pushWaitThreshold = threshold;
	}
	
	/**
	 * \brief Return the number of items in the queue.
	 * 
	 * \note Threadsafe
	 */
	inline std::size_t size() {
		lock_t lock(queueMutex);
		
		return queue.size();
	}
	
	/**
	 * \brief Return but do not pop an item from the queue.
	 * 
	 * \note Threadsafe
	 */
	inline T peak() {
		lock_t lock(queueMutex);
		
		return queue.front();
	}
	
	/**
	 * \brief Pop an item from the queue.
	 * 
	 * It is expected that code calls size() first to determine if a call
	 * to pop() will be valid.
	 * 
	 * \note Threadsafe
	 */
	inline T pop() {
		lock_t lock(queueMutex);
		
		assert(!queue.empty());
		
		// This is to get around an oversight in the C++11 standard where our std::queue
		// cannot get an item from the queue by move, only by reference. Obviously this
		// means we normally cannot use it with move-only objects, but this hack allows
		// us to use the queue with move-only objects. This is safe because we are holding
		// a lock on the mutex so we know nothing will change between a call to front()
		// and a call to pop().
		T item = std::move(const_cast<T&>(queue.front()));
		queue.pop();
		return item;
	}
	
	/**
	 * \brief Pop all the items current within the queue and return them in a queue of
	 * their own.
	 * 
	 * Safe to call when the queue is empty, as it simply returns an empty queue.
	 *
	 * \note Threadsafe
	 */
	inline std::queue<T> pop_all() {
		lock_t lock(queueMutex);
		
		std::queue<T> returnQueue;
		std::swap(queue, returnQueue);
		
		return returnQueue;
	}
	
	/**
	 * \brief Push some data into the queue.
	 * 
	 * \note Threadsafe
	 */
	inline void push(T&& item) {
		lock_t lock(queueMutex);
		
		queue.push(std::move(item));
		
		if(pushWaitThreshold > 0 && ++pushWaitNew >= pushWaitThreshold) {
			pushWaitCV.notify_all();
		}
	}
	
	/**
	 * \brief Block up to a specified amount of time waiting for an item
	 * to be pushed into the queue.
	 * 
	 * If an item is pushed into the queue before the specified
	 * amount of time then we return, otherwise we return after the
	 * specified amount of time as a timeout.
	 * 
	 * \returns Whether or not an item was pushed into the queue during
	 * our waiting.
	 * 
	 * \note Threadsafe
	 */
	inline bool push_wait(const std::size_t milliseconds) {
		lock_t lock(queueMutex);
		
		if(queue.size() >= pushWaitThreshold) {
			return true;
		}
		
		if(pushWaitCV.wait_for(lock, std::chrono::milliseconds(milliseconds), [this] {
				return pushWaitNew >= pushWaitThreshold;
				})) {
			pushWaitNew = 0;
			return true;
		}
		
		return false;
	}
	
	template <std::condition_variable& cv, std::mutex& mutex, const std::size_t milliseconds>
			bool push_wait() {
		lock_t lock(mutex);
		
		if(queue.size() >= pushWaitThreshold) {
			return true;
		}
		
		if(cv.wait_for(lock, std::chrono::milliseconds(milliseconds), [this] {
				return pushWaitNew >= pushWaitThreshold;
				})) {
			pushWaitNew = 0;
			return true;
		}
		
		return false;
	}

 private:
	std::queue<T> queue;
	std::mutex queueMutex;
	std::condition_variable pushWaitCV;
	
	std::size_t pushWaitNew;
	std::size_t pushWaitThreshold;
};

/**
 * \brief Buffer of incoming requests (for transmission).
 */
class incomingBuffer_t : public buffer<interpreted_request> {};

/**
 * \brief Buffer of outgoing replies (for transmission).
 */
class outgoingBuffer_t : public buffer<push_message> {};

#endif
