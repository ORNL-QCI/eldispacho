#ifndef _PROCESSOR_HPP
#define _PROCESSOR_HPP

#include <common.hpp>
#include "action.hpp"
#include "diagnostics/logger.hpp"
#include "model/network.hpp"
#include "model/state.hpp"
#include "simulator/adapter.hpp"
#include "simulator/client_pool.hpp"
#include "buffer.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <zmq.hpp>

/**
 * \brief The maximum number of processing threads.
 */
#define PROCESSOR_MAX_THREADS 4


#define PROCESSOR_WORK_WAIT 15 // milliseconds

/**
 * \brief Processes incoming requests and generates outgoing replies.
 */
class processor {
 private:
	typedef std::unique_lock<std::mutex> lock_t;
 
 public:
	/**
	 * \brief Constructor.
	 */
	processor(::diagnostics::logger* const logger,
			const char* const simulatorEndpoint,
			model::state& state,
			::zmq::context_t& context);
	
	/**
	 * \brief Copy constructor is disabled.
	 */
	processor(const processor&) = delete;
	
	/**
	 * \brief Move constructor is disabled.
	 */
	processor(processor&&) = delete;
	
	/**
	 * \brief Assignment operator is disabled.
	 */
	processor& operator=(const processor&) = delete; 
	
	/**
	 * \brief Move assignment operator is disabled.
	 */
	processor& operator=(processor&&) = delete;
	
	/**
	 * \brief Destructor stops processing if we are still running.
	 */
	~processor();
	
	/**
	 * \brief Start processing with a particular thread count.
	 * 
	 * The maximum number of threads is given by PROCESSOR_MAX_THREADS.
	 * 
	 * \note Threadsafe
	 */
	void start(const std::size_t threadCount);
	
	/**
	 * \brief Stop processing and terminate threads.
	 * 
	 * \note Threadsafe
	 */
	UNROLL_LOOP
	void stop();
	
	/**
	 * \brief Preprocess a request for tx.
	 * 
	 * This validates the to and from fields. 
	 * 
	 * If a client is not found then an exception is thrown.
	 */
	interpreted_request preprocess(const action type,
			const ::model::node::id_t from,
			const char* const component,
			const char* const dialect,
			const char* const circuit,
			const char lineDelimiter) {
		return interpreted_request(type,
				st.network().find_node(from),
				component,
				dialect,
				circuit,
				lineDelimiter,
				st.sim_time().now());
	}
	
	/**
	 * \brief Return a reference to the incoming buffer.
	 */
	inline incomingBuffer_t& incoming_buffer() {
		return incomingBuffer;
	}
	
	/**
	 * \brief Return a reference to the outgoing buffer.
	 */
	inline outgoingBuffer_t& outgoing_buffer() {
		return outgoingBuffer;
	}

 private:
	/**
	 * \brief An optional logger that we use to log events while processing.
	 */
	::diagnostics::logger* logger;
	
	/**
	 * \brief The state of the network.
	 */
	model::state& st;
	
	/**
	 * \brief The buffer containing items that are to be processes. The server fills this.
	 */
	incomingBuffer_t incomingBuffer;
	
	/**
	 * \brief The buffer containing items that have already been processed. The server
	 * empties this.
	 */
	outgoingBuffer_t outgoingBuffer;
	
	/**
	 * \brief The endpoint of our simulator.
	 */
	char* simulatorEndpoint;
	
	/**
	 * \brief A pool of clients connected to the simulator.
	 */
	::simulator::client_pool simulatorClientPool;
	
	/**
	 * \brief The worker threads that run the processing function.
	 */
	std::thread workerThreads[PROCESSOR_MAX_THREADS];
	
	/**
	 * \brief The number of worker threads.
	 */
	std::size_t threadCount;
	
	/**
	 * \brief Mutex to protect resources when starting and stopping processing.
	 */
	std::mutex stateChangeMutex;
	
	/**
	 * \brief Whether or not we are currently processing.
	 */
	std::atomic_bool isRunning;
	
	/**
	 * \brief Whether or not we are signalling to our processing threads that we want them
	 * to stop and return.
	 */
	std::atomic_bool doExit;
	
	/**
	 * \brief The work function that each worker thread executes.
	 */
	void work(const std::size_t id, ::simulator::client& client);
};

#endif
