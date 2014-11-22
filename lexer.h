#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#include <string>
#include <memory>

namespace mpl {

class Reader;
class Token;

class Lexer {
public:
	Lexer(::mpl::Reader& reader) {}
	virtual ~Lexer() {}

	virtual const Token& next() = 0;
	virtual const Token& lookahead() = 0;
};

std::shared_ptr<Lexer> create_lexer(const std::string& type, ::mpl::Reader& reader);

} // namespace mpl

#endif // MPL_LEXER_H