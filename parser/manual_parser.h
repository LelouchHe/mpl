#ifndef MPL_PARSER_MANUAL_PARSER_H
#define MPL_PARSER_MANUAL_PARSER_H

#include <map>
#include <string>

namespace mpl {

class Reader;

namespace parser {

// Lexer需要满足的具体要求,见 lexer.h
template <typename Lexer>
class ManualParser {
public:
	ManualParser(::mpl::Reader& reader);
	~ManualParser();

public:
	typedef typename Lexer::Token Token;
	typedef typename Token::TokenType TokenType;

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