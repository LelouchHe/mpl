#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#include <string>
#include <memory>

namespace mpl {

class Reader;
class Token;

class Lexer {
public:
	Lexer(::mpl::Reader& reader) : _reader(reader) {}
	virtual ~Lexer() {}

	virtual const Token& next() = 0;
	virtual const Token& lookahead() = 0;

protected:
	Reader& _reader;
};

std::shared_ptr<Lexer> create_lexer(const std::string& type, Reader& reader);

} // namespace mpl

#endif // MPL_LEXER_H