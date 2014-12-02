#ifndef MPL_LEXER_AUTO_LEXER_H
#define MPL_LEXER_AUTO_LEXER_H

#include <sstream>
#include "token.h"

namespace mpl {
class Reader;

namespace lexer {

class AutoLexer {
public:
	AutoLexer(::mpl::Reader& reader);
	~AutoLexer();

public:
	typedef Token Token;
	typedef Token::TokenType TokenType;

	const Token& next();
	const Token& lookahead();
	static TokenType token_type(const std::string& name);
	static const std::string& token_name(TokenType type);

private:
	TokenType lex();

private:
	::mpl::Reader& _reader;
	std::ostringstream _buf;
	char _current;

	Token _next;
	Token _ahead;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_AUTO_LEXER_H