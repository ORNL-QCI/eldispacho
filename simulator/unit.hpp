#ifndef _SIMULATOR_UNIT_HPP
#define _SIMULATOR_UNIT_HPP

#include <common.hpp>

/**
 * \brief \todo
 */
#define SIMULATOR_UNIT_DIALECT_POOL_SIZE 8

namespace simulator {
	/**
	 * \brief \todo
	 */
	struct unit {
	 public:
		/**
		 * \brief \todo
		 */
		static void convert_line_delimiter(char* const str,
				const char oldCh,
				const char newCh);
	
	 private:
		static struct pool_t {
			static char* dialect[SIMULATOR_UNIT_DIALECT_POOL_SIZE];
			
			/**
			 * \brief \todo
			 */
			~pool_t() {
				std::size_t i = 0;
				while(dialect[i] != 0) {
					delete[] dialect[i];
					i++;
				}
			}
			
			/**
			 * \brief \todo
			 */
			template <char* ary[]> inline const char* get(const char* const str) {
				for(std::size_t i = 0; i < SIMULATOR_UNIT_DIALECT_POOL_SIZE; i++) {
					if(ary[i] == 0) {
						std::size_t len = strlen(str) + 1;
						ary[i] = new char[len];
						memcpy(ary[i], str, len);
						return ary[i];
					} else if(strcmp(ary[i], str) == 0) {
						return ary[i];
					}
					i++;
				}
				
				throw std::runtime_error(err_msg::_arybnds);
			}
			
			/**
			 * \brief \todo
			 */
			const char* get_dialect(const char* const str) {
				return get<dialect>(str);
			}
		} _pool;
	
	 public:
		/**
		 * \brief \todo
		 */
		unit(const char* const dialect,
				const char* const description,
				const char lineDelimiter,
				const bool doAllocate = false,
				const bool usePool = true);
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		unit(const unit&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		unit(unit&& old);
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		unit& operator=(const unit&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		unit& operator=(unit&& old);
		
		/**
		 * \brief \todo
		 */
		~unit();
		
		/**
		 * \brief \todo
		 */
		inline bool isNull() const {
			return (_dialect == 0 || _description == 0);
		}
		
		/**
		 * \brief \todo
		 */
		inline const char* dialect() const {
			return _dialect;
		}
		
		/**
		 * \brief \todo
		 */
		inline const char* description() const {
			return _description;
		}
		
		/**
		 * \brief \todo
		 */
		inline char line_delimiter() const {
			return _lineDelimiter;
		}
	
	 private:
		/**
		 * \brief \todo
		 */
		char* _dialect;
		
		/**
		 * \brief \todo
		 */
		char* _description;
		
		/**
		 * \brief \todo
		 */
		char _lineDelimiter;
		
		/**
		 * \brief \todo
		 */
		bool _isAllocated;
		
		/**
		 * \brief \todo
		 */
		bool _usePool;
	};
}

#endif
