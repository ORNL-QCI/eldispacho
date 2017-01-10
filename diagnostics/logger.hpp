#ifndef _DIAGNOSTICS_LOGGER_HPP
#define _DIAGNOSTICS_LOGGER_HPP

#include <common.hpp>
#include "message.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace diagnostics {
	/**
	 * \brief \todo
	 */
	class logger {
	 private:
		/**
		 * \brief Alias declaration for standard mutex lock type.
		 */
		using lock_t = std::lock_guard<std::mutex>;
	
	 public:
		/**
		 * \brief Constructor.
		 * 
		 * If isRealized is false then the logger is just a dummy and doesn't run.
		 */
		logger(const bool isRealized);
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		logger(const logger&) = delete;
		
		 /**
		 * \brief Move constructor is disabled.
		 */
		logger(logger&&) = delete;
		
		 /**
		 * \brief Assignment operator is disabled.
		 */
		logger& operator=(const logger&) = delete; 
		
		 /**
		 * \brief Move assignment operator is disabled.
		 */
		logger& operator=(logger&&) = delete;
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~logger();
		
		/**
		 * \brief Start the logger logging.
		 */
		void start();
		
		/**
		 * \brief Stop the logger logging.
		 */
		void stop();
		
		/**
		 * \brief Push a message into the queue or do nothing, depending on how the
		 * logger has been configured.
		 * 
		 * \warning Should not contain a null-terminator.
		 * 
		 * \note Threadsafe.
		 */
		void put(const action topic,
				const void* const data,
				const std::size_t size);
		
		/**
		 * \brief Set the threshold when a call to push_wait will be successful.
		 * 
		 * \note Threadsafe
		 */
		inline void set_push_wait_threshold(const std::size_t threshold) {
			lock_t lock(sendQueueMutex);
			
			pushWaitThreshold = threshold;
		}
		
		/**
		 * \brief Wait until the push wait threshold is exceeded.
		 * 
		 * If the current size of the queue is larger than the threshold, we return
		 * immediately.
		 * 
		 * \note Threadsafe
		 */
		bool push_wait(const std::size_t milliseconds);
		
		/**
		 * \brief Return the size of the queue.
		 * 
		 * \note Threadsafe
		 */
		inline std::size_t size() {
			lock_t lock(sendQueueMutex);
			
			return sendQueue.size();
		}
	
	 protected:
		/**
		 * \brief Whether or not the logger should really put.
		 */
		bool isRealized;
		
		/**
		 * \brief Whether or not the logger is running.
		 */
		bool isRunning;
		
		/**
		 * \brief Exit flag to signal to worker thread to exit.
		 */
		std::atomic_bool doExit;
		
		/**
		 * \brief Mutex protector of the logger state.
		 */
		std::mutex editMutex;
		
		/**
		 * \brief The thread that the implemented work() function executes.
		 */
		std::thread workerThread;
		
		/**
		 * \brief The queue of logger messages to be logged.
		 */
		std::queue<message*> sendQueue;
		
		/**
		 * \brief The condition variable used in push_wait() that.
		 */
		std::condition_variable sendQueueCV;
		
		/**
		 * \brief The number of items pushed into the queue since the last call to
		 * push_wait() where we exceeded the push wait threshold.
		 */
		std::size_t pushWaitNew;
		
		/**
		 * \brief The number of new items pushed into the queue until a call to
		 * push_wait() returns true.
		 */
		std::size_t pushWaitThreshold;
		
		/**
		 * \brief The protector of the queue.
		 */
		std::mutex sendQueueMutex;
		
		/**
		 * \brief The work function that pops items from the queue and logs them.
		 * 
		 * Virtualality is insignificant to performance since this function is only called
		 * once on start().
		 */
		virtual void work() = 0;
		
		/**
		 * \brief Pop all the items current within the queue and return them in a queue of
		 * their own.
		 * 
		 * Safe to call when the queue is empty, as it simply returns an empty queue.
		 *
		 * \note Threadsafe
		 */
		std::queue<message*> pop_all();
		
		/**
		 * \brief Function pointer to either _put or _put_nothing.
		 * 
		 * We don't use virtual functions here.
		 */
		void (logger::*_put_func)(const action topic,
				const void* const data,
				const std::size_t size);
		
		/**
		 * \brief Push a message into the queue.
		 * 
		 * \warning Should not contain a null-terminator.
		 * 
		 * \note Threadsafe.
		 */
		void _put(const action topic,
				const void* const data,
				const std::size_t size);
		
		/**
		 * \brief Don't do anything.
		 * 
		 * \note Threadsafe.
		 */
		void _put_nothing(const action topic,
				const void* const data,
				const std::size_t size);
	};
}

#endif
