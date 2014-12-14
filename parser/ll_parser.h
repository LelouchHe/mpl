#ifndef MPL_PARSER_LL_PARSER_H
#define MPL_PARSER_LL_PARSER_H

#include "../lexer.h"

namespace mpl {

class Reader;

namespace parser {

class LLParser {
public:
	LLParser(::mpl::Reader& reader);
	~LLParser();

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

#endif // MPL_PARSER_LL_PARSER_H