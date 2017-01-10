#include "logger.hpp"

namespace diagnostics {
	logger::logger(const bool isRealized)
			:isRealized(isRealized), 
			isRunning(false),
			doExit(false),
			pushWaitNew(0),
			pushWaitThreshold(0) {
		
		if(isRealized) {
			_put_func = &diagnostics::logger::_put;
		} else {
			_put_func = &diagnostics::logger::_put_nothing;
		}
	}
	
	logger::~logger() {
		stop();
	}
	
	void logger::start() {
		if(!isRealized) {
			return;
		}
		
		lock_t lock(editMutex);
		
		if(isRunning) {
			return;
		}
		
		isRunning = true;
		workerThread = std::thread(&logger::work, this);
	}
	
	void logger::stop() {
		if(!isRealized) {
			return;
		}
		
		lock_t lock(editMutex);
		
		if(!isRunning) {
			return;
		}
		
		doExit = true;
		
		workerThread.join();
		
		doExit = false;
		isRunning = false;
	}
	
	void logger::put(const action topic,
			const void* const data,
			const std::size_t size) {
		(this->*_put_func)(topic, data, size);
	}
	
	bool logger::push_wait(const std::size_t milliseconds) {
		std::unique_lock<std::mutex> lock(sendQueueMutex);
		
		if(sendQueue.size() > pushWaitThreshold ||
				sendQueueCV.wait_for(lock,
						std::chrono::milliseconds(milliseconds),
						[this] {
							return pushWaitNew >= pushWaitThreshold;
						})) {
			pushWaitNew = 0;
			return true;
		}
		
		return false;
	}
	
	std::queue<message*> logger::pop_all() {
		lock_t lock(sendQueueMutex);
		
		std::queue<message*> returnQueue;
		std::swap(sendQueue, returnQueue);
		
		return returnQueue;
	}
	
	void logger::_put(const action topic,
			const void* const data,
			const std::size_t size) {
		#ifdef THROW
		if(UNLIKELY(size == 0)) {
			throw std::invalid_argument(err_msg::_zrlngth);
		}
		#endif
		
		std::unique_lock<std::mutex> ulock(sendQueueMutex);
		
		// By allocating this message on the heap and storing the pointer, we only need to
		// do one allocation for the message object until we push this out on the wire.
		sendQueue.push(new message(topic, (char*)data, size));
		
		// Avoid waking up waiting threads to block again if notify_all() is called.
		ulock.unlock();
		
		
		// Should this even be done???
		if(pushWaitThreshold > 0 && ++pushWaitNew >= pushWaitThreshold) {
			sendQueueCV.notify_all();
		}
	}
	
	void logger::_put_nothing(const action topic,
			const void* const data,
			const std::size_t size) {
		UNUSED(topic);
		UNUSED(data);
		UNUSED(size);
		
		return;
	}
}
