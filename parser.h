#ifndef MPL_PARSER_H
#define MPL_PARSER_H

#include <map>
#include <string>

namespace mpl {

class Reader;

// Lexer需要满足的具体要求,见 lexer.h
template <typename Lexer>
class Parser {
public:
	Parser(::mpl::Reader& reader);
	~Parser();

public:
	typedef typename Lexer::Token Token;
	typedef typename Lexer::TokenType TokenType;

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

} // namespace mpl

#endif // MPL_PARSER_H