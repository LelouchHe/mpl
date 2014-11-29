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

	// �ж��ļ�����,�м���:
	// 1. feof: ȷʵ��ȡ��EOF֮��,�Ż᷵��true,�޷����ж�
	// 2. peek() == EOF: ������ǰ�ж��Ƿ��β,������lookaheadʱ,�ᵼ����һ���Ѿ���ȡ,��δ������ַ�������(�����������Ļ�)
	// 3. ����current,�ж�current==EOF: �����������lookahead,���ᶪ���ַ�
	virtual bool eof() {
		return _current == EOF;
	}

private:
	std::ifstream _ifs;
	char _current;
};

} // namespace mpl

#endif // MPL_FILE_READER_H