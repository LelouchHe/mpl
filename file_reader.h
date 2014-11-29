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
	~FileReader() {

	}

public:
	virtual char next() {
		_current = _ifs.get();
		return _current;
	}

	// 判断文件结束,有几种:
	// 1. feof: 确实读取到EOF之后,才会返回true,无法遇判断
	// 2. peek() == EOF: 可以提前判断是否结尾,但用作lookahead时,会导致上一次已经读取,尚未处理的字符被丢弃(如果单看这个的话)
	// 3. 保存current,判断current==EOF: 这样方便进行lookahead,不会丢弃字符
	virtual bool eof() {
		return _current == EOF;
	}

private:
	std::ifstream _ifs;
	char _current;
};

} // namespace mpl

#endif // MPL_FILE_READER_H