#include "unit.hpp"

namespace simulator {
	void unit::convert_line_delimiter(char* const str,
			const char oldCh,
			const char newCh) {
		char* pch = strchr(str, oldCh);
		while(pch != 0) {
			*pch = newCh;
			pch = strchr(str, oldCh);
		}
	}
	
	char* unit::pool_t::dialect[SIMULATOR_UNIT_DIALECT_POOL_SIZE] = {0};
	
	unit::pool_t unit::_pool;
	
	unit::unit(const char* const dialect,
			const char* const description,
			const char lineDelimiter,
			const bool doAllocate,
			const bool usePool) :
			_lineDelimiter(lineDelimiter),
			_isAllocated(doAllocate),
			_usePool(usePool) {
		if(doAllocate) {
			auto len = strlen(description) + 1;
			_description = new char[len];
			memcpy(_description, description, len);
			if(usePool) {
				_dialect = const_cast<char*>(_pool.get_dialect(dialect));
				
			} else {
				len = strlen(dialect) + 1;
				_dialect = new char[len];
				memcpy(_dialect, dialect, len);
			}
		} else {
			_dialect = const_cast<char*>(dialect);
			_description = const_cast<char*>(description);
		}
	}
	
	unit::unit(unit&& old)
			: _dialect(old._dialect),
			_description(old._description),
			_lineDelimiter(old._lineDelimiter),
			_isAllocated(old._isAllocated),
			_usePool(old._usePool) {
		old._dialect = 0;
		old._description = 0;
	}
	
	unit& unit::operator=(unit&& old) {
		_dialect = old._dialect;
		old._dialect = 0;
		_description = old._description;
		old._description = 0;
		_lineDelimiter = old._lineDelimiter;
		_isAllocated = old._isAllocated;
		_usePool = old._usePool;
		
		return *this;
	}
	
	unit::~unit() {
		if(_isAllocated && !_usePool) {
			if(_dialect != 0) {
				delete[] _dialect;
			}
			if(_description != 0) {
				delete[] _description;
			}
		}
	}
}
