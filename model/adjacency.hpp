#ifndef _MODEL_ADJACENCY_HPP
#define _MODEL_ADJACENCY_HPP

#include <common.hpp>
#include <boost/multi_array.hpp>

namespace model {
	/**
	 * \brief An adjacency matrix with various helper functions.
	 */
	template <typename T> struct adjacency {
	 public:
		/**
		 * \brief Constructor takes an initial size of the square matrix.
		 */
		adjacency<T>(const std::size_t size)
				: _matrix(boost::extents[size][size]) {
		}
		
		/**
		 * \brief Copy constructor disabled.
		 */
		adjacency<T>(const adjacency<T>&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		adjacency<T>(adjacency<T>&& old)
				: _matrix(std::move(old._matrix)) {
		}
		
		/**
		 * \brief Copy Assignment operator disabled.
		 */
		adjacency<T>& operator=(const adjacency<T>&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		adjacency<T>& operator=(adjacency<T>& old) {
			_matrix = old._matrix;
		}
		
		/**
		 * \brief Virtual Destructor.
		 */
		virtual ~adjacency<T>() {
		}
		
		/**
		 * \brief Add an item at position (i,j). If an element is already at this position,
		 * overwrite it.
		 * 
		 * The template parameter allowNull is a way to force us to think about what the
		 * effect allowing a null item pointer will have.
		 */
		template <bool allowNull>
				void add(const std::size_t i, const std::size_t j, T* const item) {
			#ifdef THROW
			if(UNLIKELY(i >= _matrix.size() || j >= _matrix.size())) {
				throw std::out_of_range(err_msg::_arybnds);
			}
			if(!allowNull && UNLIKELY(item == 0)) {
				throw std::invalid_argument(err_msg::_nllpntr);
			}
			#endif
			
			_matrix[i][j] = item;
		}
		
		/**
		 * \brief Remove an item at the position (i,j). If there is no item at this
		 * position, then this has no effect.
		 */
		void remove(const std::size_t i, const std::size_t j) {
			#ifdef THROW
			if(UNLIKELY(i >= _matrix.size() || j >= _matrix.size())) {
				throw std::out_of_range(err_msg::_arybnds);
			}
			#endif
			
			_matrix[i][j] = 0;
		}
		
		/**
		 * \brief Determine if there is an item at the position (i,j).
		 */
		inline bool is_empty(const std::size_t i, const std::size_t j) const {
			#ifdef THROW
			if(UNLIKELY(i >= _matrix.size() || j >= _matrix.size())) {
				throw std::out_of_range(err_msg::_arybnds);
			}
			#endif
			
			return (_matrix[i][j] == 0);
		}
		
		/**
		 * \brief Return a pointer to the element positioned at (i,j) within the matrix. If
		 * there is no item present, the pointer is equal to 0.
		 * 
		 * \returns The pointer to the element at position (i,j).
		 */
		inline T* pget(const std::size_t i, const std::size_t j) const {
			#ifdef THROW
			if(UNLIKELY(i >= _matrix.size() || j >= _matrix.size())) {
				throw std::out_of_range(err_msg::_arybnds);
			}
			#endif
			
			return _matrix[i][j];
		}
		
		/**
		 * \brief Return a mutable reference to the item positioned at (i,j) within the
		 * matrix.
		 * 
		 * \throws std::runtime_error if the element at the position is null.
		 */
		inline T& rget(const std::size_t i, const std::size_t j) {
			#ifdef THROW
			if(UNLIKELY(i >= _matrix.size() || j >= _matrix.size())) {
				throw std::out_of_range(err_msg::_arybnds);
			}
			#endif
			
			if(UNLIKELY(_matrix[i][j] == 0)) {
				throw std::runtime_error(err_msg::_arybnds);
			}
			
			return *_matrix[i][j];
		}
		
		/**
		 * \brief Resize the square matrix to a new size.
		 * 
		 * If the new size is smaller than the current size, pointers outside the new size
		 * of the matrix are deleted. Their memory is not deallocated from this function.
		 * If the new size is bigger than the current size, the new positions are
		 * initialized to 0.
		 */
		void resize(const std::size_t size) {
			_matrix.resize(boost::extents[size][size]);
		}
		
		/**
		 * \brief Return the size of the square matrix.
		 */
		inline std::size_t size() const {
			return _matrix.size();
		}
		
		/**
		 * \brief Return a pointer to the next neighbor of an item.
		 * 
		 * Since an item's connectivity is within a row, this retrieves the first non-null
		 * element in the row with a given offset. For example, if the offset is 0, this
		 * retrieves the first non-null element in the row. However, if the offset is 1,
		 * this retrieves the second non-null element in the row. If there is only one
		 * non-null element in the row, this retrieves the first non-null element in the
		 * row. If the offset is greater than the number of non-null elements in the row,
		 * this retrieves the last non-null element in the row. If there are no non-null
		 * elements in the row, then 0 is returned.
		 */
		T* next_neighbor(const std::size_t i, std::size_t offset = 0) const {
			#ifdef THROW
			if(UNLIKELY(i >= _matrix.size())) {
				throw std::out_of_range(err_msg::_arybnds);
			}
			#endif
			
			T* lastElement = 0;
			
			for(std::size_t j = 0; j < _matrix.size(); j++) {
				if(_matrix[i][j] != 0) {
					if(offset == 0) {
						lastElement = _matrix[i][j];
						break;
					} else {
						lastElement = _matrix[i][j];
						offset--;
					}
				}
			}
			
			return lastElement;
		}
		
	 private:
		/**
		 * \brief A square matrix.
		 */
		boost::multi_array<T*, 2> _matrix;
	};
}

#endif
