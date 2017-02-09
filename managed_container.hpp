#ifndef _MANAGED_CONTAINER_HPP
#define _MANAGED_CONTAINER_HPP

#include <common.hpp>
#include "utility.hpp"

/**
 * \brief \todo
 */
template <typename T,
	typename U = std::size_t,
	U stackCapacityT = 1,
	bool isItemOwner = false>
		class managed_container {
	
	// The size of the stack buffer is rounded to a multiple of the wordsize, i.e. it
	// is 4 in the case of a 32bit system and 8 in the case of a 64 bit system.
	static const U stackCapacity =
			utility::round_to_multiple<std::size_t>(sizeof(std::size_t), stackCapacityT);
	
	// T must be an integral type, either built-in or implementation defined.
	static_assert(std::is_integral<U>::value, "T must be numeric type");
	
	// We need the stack buffer to be at least as large as the size of the numeric
	// type because when we switch to heap storage, the size of the heap is stored
	// within the stack buffer.
	static_assert(stackCapacity >= sizeof(U), "stack capacity too small");
	
	// We don't want to delete a non-pointer type, so this provides a clear warning
	// when the given template parameters are conflicting.
	static_assert((!std::is_pointer<T>::value && isItemOwner) ? false : true,
			"isItemOwner must be false if T is not a pointer type");

 public:
	/**
	 * \brief Empty constructor initializes the stack buffer with the given capacity.
	 */
	managed_container()
			: _count(0),
			_heap(0),
			_stack{} {
	}
	
	/**
	 * \brief Copy constructor is disabled.
	 */
	managed_container(const managed_container&) = delete;
	
	/**
	 * \brief Move constructor.
	 */
	managed_container(managed_container&& old)
			: _count(old._count),
			_heap(old._heap) {
		old._heap = 0;
		memcpy(_stack, old._stack,  stackCapacity);
		
		// Stop us trying to deallocate items after they have been moved (only applies
		// if we are the owner).
		old._count = 0;
	}
	
	/**
	 * \brief Assignment operator is disabled.
	 */
	managed_container& operator=(const managed_container&) = delete;
	
	/**
	 * \brief Move assignment operator.
	 */
	managed_container& operator=(managed_container&& old) {
		_count = old._count;
		_heap = old._heap;
		old._heap = 0;
		memcpy(_stack, old._stack, stackCapacity);
		
		// Stop us trying to deallocate items after they have been moved (only applies
		// if we are the owner).
		old._count = 0;
		
		return *this;
	}
	
	/**
	 * \brief Destructor deallocates any heap memory.
	 */
	virtual ~managed_container() {
		// This call will be non-trivial if isItemOwner is true
		deallocate();
		
		if(_heap != 0) {
			delete[] _heap;
		}
	}
	
	/**
	 * \brief Return the number of items of size sizeof(T) stored.
	 */
	constexpr inline U count() const {
		return _count;
	}
	
	/**
	 * \brief Return the number of items which may be stored without reallocation.
	 * 
	 * This does not take into account what is already stored!
	 */
	inline U capacity() const {
		if(is_heap()) {
			U tempCapacity = 0;
			memcpy(&tempCapacity, _stack, sizeof(U));
			return tempCapacity;
		}
		
		return stackCapacity;
	}
	
	/**
	 * \brief Return the number of items that may be stored before an allocation.
	 */
	inline U remaining() const {
		return capacity() - count();
	}
	
	/**
	 * \brief Return a pointer to the immutable data.
	 */
	inline const T* data() const {
		return (is_stack() ? _stack : _heap);
	}
	
	/**
	 * \brief Copy a chunk of data to the buffer.
	 */
	void add(const T* const buf, const U count) {
		#ifdef ELDISPACHO_THROW
		if(UNLIKELY(buf == 0)) {
			throw std::invalid_argument(err_msg::_nllpntr);
		}
		if(UNLIKELY(count == 0)) {
			throw std::invalid_argument(err_msg::_zrlngth);
		}
		if(UNLIKELY(utility::memory_overlap<T>(mdata(), _count, buf, count))) {
			throw std::invalid_argument(err_msg::_ovrlmem);
		}
		#endif
		
		const U newSize = (_count+count)*sizeof(T);
		
		if(newSize < capacity()) {
			// We can fit in the existing buffer
			memcpy(&mdata()[_count], buf, count*sizeof(T));
		} else {
			// We need to do some heap allocation
			const U newCapacity =
					utility::round_to_multiple<std::size_t, sizeof(std::size_t)>(newSize);
			
			T* newHeap = new T[newCapacity];
			// Copy old data
			memcpy(newHeap, mdata(), _count*sizeof(T));
			// Copy new data
			memcpy(&newHeap[_count], buf, count*sizeof(T));
			
			if(is_heap()) {
				delete[] _heap;
			}
			
			// Set new capacity
			memcpy(_stack, &newCapacity, sizeof(newCapacity));
			// Save new buffer location
			_heap = newHeap;
		}
		
		_count += count;
	}
	
	/**
	 * \brief Remove a given number of items at the end of the buffer.
	 * 
	 * \throws If the exception throw macro is defined, this will throw
	 * std::invalid_argument() if the given trim number ss greater than the number of
	 * items currently stored, which would result in an underflow upon subtraction.
	 */
	inline void trim(const U count) {
		#ifdef ELDISPACHO_THROW
		if(UNLIKELY(_count < count)) {
			throw std::invalid_argument(err_msg::_arybnds);
		}
		#endif
		
		_count -= count;
	}

 private:
	/**
	 * \brief The number of items stored.
	 */
	U _count;
	
	/**
	 * \brief A pointer to the heap buffer.
	 * 
	 * If the heap buffer is not used, this is equal to 0.
	 */
	T* _heap;
	
	/**
	 * \brief The stack buffer.
	 * 
	 * If the heap is used, the first sizeof(U) bytes represents the capacity of
	 * the heap buffer. The minimum size of this array has been guaranteed previously.
	 */
	T _stack[stackCapacity];
	
	/**
	 * \brief Return whether or not we are using a heap buffer.
	 */
	inline bool is_heap() const {
		return (_heap != 0);
	}
	
	/**
	 * \brief Return whether or not we are using a stack buffer.
	 */
	inline bool is_stack() const {
		return !is_heap();
	}
	
	/**
	 * \brief Return a pointer to the mutable buffer.
	 * 
	 * \warning This doesn't perform any sort of bounds checking, that is delegated to
	 * the caller of this function.
	 */
	inline T* mdata() {
		return (is_heap() ? _heap : _stack);
	}
	
	/**
	 * \brief Deallocate each item because we own them.
	 * 
	 * The implementation of this function depends on the template parameter
	 * isItemOwner and thus relies on SFINAE. See the other definition of deallocate()
	 * for the implementation when we do not own items.
	 */
	template <bool _isItemOwner = isItemOwner,
			typename std::enable_if<_isItemOwner, bool>::type = true>
			inline void deallocate() {
		auto dataP = mdata();
		
		for(U i = 0; i < _count; i++) {
			delete dataP[i];
		}
	}
	
	/**
	 * \brief Don't deallocate each item because we do not own them.
	 *
	 * The implementation of this function depends on the template parameter
	 * isItemOwner and thus relies on SFINAE. See the other definition of deallocate()
	 * for the implementation when we do own items.
	 */
	template <bool _isItemOwner = isItemOwner,
			typename std::enable_if<!_isItemOwner, bool>::type = true>
		constexpr inline void deallocate() const {
	}
};

#endif
