#ifndef MPL_PARSER_AUTO_PARSER_H
#define MPL_PARSER_AUTO_PARSER_H

#include "../config.h"

namespace mpl {

class Reader;

namespace parser {

class AutoParser {
public:
	AutoParser(::mpl::Reader& reader);
	~AutoParser();

public:
	typedef ::mpl::Lexer Lexer;

	typedef Lexer::Token Token;
	typedef Token::TokenType TokenType;

	void parse();

private:
	Lexer _lexer;
};
	
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_AUTO_PARSER_H