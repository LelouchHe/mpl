#ifndef MPL_STRING_READER_H
#define MPL_STRING_READER_H

#include <string>

#include "reader.h"

namespace mpl {

class StringReader : public Reader {
public:
	StringReader(const std::string& str) :
			_str(str), _index(0), _current('\0') {

	}
	~StringReader() {

	}

public:
	virtual char next() {
		if (_index < _str.size()) {
			_current = _str[_index++];
		} else {
			_current = '\0';
		}
		
		return _current;
	}

	virtual bool eof() {
		return _index == _str.size() && _current == '\0';
	}

private:
	std::string _str;
	size_t _index;
	char _current;
};

} // namespace mpl

#endif // MPL_STRING_READER_H