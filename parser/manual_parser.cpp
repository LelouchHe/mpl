#include "manual_parser.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

namespace mpl {
namespace parser {

ManualParser::ManualParser(::mpl::Reader& reader) :
	_lexer(reader) {
}

ManualParser::~ManualParser() {

}

#if 0

小写是语法,大写和标号是最终符号

expr  -> ID '=' value | '=' value
value -> prod {'+' prod} | prod {'-' prod}
prod  -> term {'*' term} | term {'/' term}
term  -> NUM | ID | '(' value ')'

#endif

int ManualParser::term() {
	Token t = _lexer.next();

	switch (t.type) {
	case TokenType::TT_NUMBER:
		return strtol(t.text.c_str(), NULL, 0);

	case TokenType::TT_ID:
		assert(_vars.find(t.text) != _vars.end());
		return _vars[t.text];

	case TokenType::TT_LEFT_PARENTHESIS:
		{
			int n = value();
			t = _lexer.next();
			assert(t.type == TokenType::TT_RIGHT_PARENTHESIS);
			return n;
		}

	default:
		break;
	}

	assert(false);
	return 0;
}

int ManualParser::prod() {
	int n = term();

	Token ahead = _lexer.lookahead();
	while (ahead.type == TokenType::TT_MUL || ahead.type == TokenType::TT_DIV) {
		_lexer.next();
		if (ahead.type == TokenType::TT_MUL) {
			n *= prod();
		} else {
			n /= prod();
		}
		ahead = _lexer.lookahead();
	}

	return n;
}

int ManualParser::value() {
	int n = prod();

	Token ahead = _lexer.lookahead();
	while (ahead.type == TokenType::TT_PLUS || ahead.type == TokenType::TT_MINUS) {
		_lexer.next();
		if (ahead.type == TokenType::TT_PLUS) {
			n += prod();
		} else {
			n -= prod();
		}
		ahead = _lexer.lookahead();
	}

	return n;
}

void ManualParser::expr() {
	Token t = _lexer.next();
	if (t.type == TokenType::TT_ASSIGN) {
		std::cout << value() << std::endl;
	} else if (t.type == TokenType::TT_ID) {
		Token assign = _lexer.next();
		assert(assign.type == TokenType::TT_ASSIGN);
		int s = value();

		_vars[t.text] = s;
	}
}

void ManualParser::parse() {
	while (_lexer.lookahead().type != TokenType::EOS) {
		expr();
	}
}

} // namespace parser
} // namespace mpl

#if 0

#include <iostream>
#include "../file_reader.h"

using namespace std;

int main() {
	const char* file_name = "lexer.txt";

	::mpl::FileReader reader(file_name);

	::mpl::parser::ManualParser parser(reader);

	parser.parse();

	return 0;
}

#endif