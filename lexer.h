#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#include <string>

namespace mpl {

class Token;

class Lexer {
public:
	virtual ~Lexer() {}

	virtual const Token& next() = 0;
	virtual const Token& lookahead() = 0;
};

} // namespace mpl

#endif // MPL_LEXER_H