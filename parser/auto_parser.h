#ifndef MPL_PARSER_AUTO_PARSER_H
#define MPL_PARSER_AUTO_PARSER_H

#include "gramma.h"

namespace mpl {

class Reader;

namespace parser {

template <typename Lexer>
class AutoParser {
public:
	AutoParser(::mpl::Reader& reader);
	~AutoParser();

public:
	typedef typename Lexer::Token Token;
	typedef typename Token::TokenType TokenType;

	void parse();

private:
	void init();

private:
	Lexer _lexer;
	Gramma<Token> _gramma;
};
	
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_AUTO_PARSER_H