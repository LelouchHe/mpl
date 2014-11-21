#ifndef MPL_PARSER_H
#define MPL_PARSER_H

#include <map>
#include <string>
#include <memory>
#include "lexer.h"

namespace mpl {

class Reader;

class Parser {
public:
	Parser(::mpl::Reader& reader);
	~Parser();

	void parse();

private:
	void expr();
	int value();
	int prod();
	int term();

private:
	std::shared_ptr<Lexer> _lexer;
	std::map<std::string, int> _vars;
};

} // namespace mpl

#endif // MPL_PARSER_H