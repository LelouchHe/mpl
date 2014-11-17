#ifndef MPL_FILE_READER_H
#define MPL_FILE_READER_H

#include "reader.h"
#include <string>
#include <fstream>

namespace mpl {

class FileReader : public Reader {
public:
	FileReader(const std::string& file_name)
		: _ifs(file_name, std::ifstream::in) {
	}

	virtual char next() {
		return _ifs.get();
	}

	virtual bool eof() {
		return _ifs.peek() == EOF;
	}

private:
	std::ifstream _ifs;
};

} // namespace mpl

#endif // MPL_FILE_READER_H