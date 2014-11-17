#ifndef MPL_READER_H
#define MPL_READER_H

namespace mpl {

class Reader {
public:
	virtual ~Reader() {
	}

	virtual char next() = 0;
	virtual bool eof() = 0;
};

} // namespace mpl

#endif // MPL_READER_H