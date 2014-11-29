#ifndef MPL_PARSER_MANUAL_PARSER_H
#define MPL_PARSER_MANUAL_PARSER_H

#include <map>
#include <string>

#include "../config.h"

namespace mpl {

class Reader;

namespace parser {

// Lexer需要满足的具体要求,见 lexer.h
class ManualParser {
public:
	ManualParser(::mpl::Reader& reader);
	~ManualParser();

public:
	typedef ::mpl::Lexer Lexer;

	typedef Lexer::Token Token;
	typedef Token::TokenType TokenType;

	void parse();

private:
	void expr();
	int value();
	int prod();
	int term();

private:
	Lexer _lexer;
	std::map<std::string, int> _vars;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_MANUAL_PARSER_H