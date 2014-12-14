#ifndef MPL_PARSER_LALR_PARSER_H
#define MPL_PARSER_LALR_PARSER_H

#include "../lexer.h"

namespace mpl {

class Reader;

namespace parser {

class LALRParser {
public:
	LALRParser(::mpl::Reader& reader);
	~LALRParser();

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

#endif // MPL_PARSER_LALR_PARSER_H