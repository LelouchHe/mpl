#include "parser.h"

#include <iostream>
#include <cstdlib>
#include <cassert>
#include "lexer.h"

namespace mpl {

Parser::Parser(::mpl::Reader& reader): 
	_lexer(reader) {
}

Parser::~Parser() {

}

#if 0

小写是语法,大写和标号是最终符号

expr  -> ID '=' value | '=' value
value -> prod {'+' prod} | prod {'-' prod}
prod  -> term {'*' term} | term {'/' term}
term  -> NUM | ID | '(' value ')'

#endif

int Parser::term() {
	::mpl::Token t = _lexer.next();

	switch (t.type) {
	case TT_NUMBER:
		return strtol(t.text.c_str(), NULL, 0);

	case TT_ID:
		assert(_vars.find(t.text) != _vars.end());
		return _vars[t.text];

	case TT_LEFT_PARENTHESIS:
		{
			int n = value();
			t = _lexer.next();
			assert(t.type == TT_RIGHT_PARENTHESIS);
			return n;
		}

	default:
		break;
	}

	assert(false);
	return 0;
}

int Parser::prod() {
	int n = term();

	::mpl::Token ahead = _lexer.lookahead();
	while (ahead.type == TT_MUL || ahead.type == TT_DIV) {
		_lexer.next();
		if (ahead.type == TT_MUL) {
			n *= prod();
		} else {
			n /= prod();
		}
		ahead = _lexer.lookahead();
	}

	return n;
}

int Parser::value() {
	int n = prod();

	::mpl::Token ahead = _lexer.lookahead();
	while (ahead.type == TT_PLUS || ahead.type == TT_MINUS) {
		_lexer.next();
		if (ahead.type == TT_PLUS) {
			n += prod();
		} else {
			n -= prod();
		}
		ahead = _lexer.lookahead();
	}

	return n;
}

void Parser::expr() {
	::mpl::Token t = _lexer.next();
	if (t.type == TT_ASSIGN) {
		std::cout << value() << std::endl;
	} else if (t.type == TT_ID) {
		::mpl::Token assign = _lexer.next();
		assert(assign.type == TT_ASSIGN);
		int s = value();

		_vars[t.text] = s;
	}
}

void Parser::parse() {
	while (_lexer.lookahead().type != TT_EOS) {
		expr();
	}
}

} // namespace mpl

#if 0

#include "file_reader.h"

int main() {
	const char* file_name = "test.txt";

	::mpl::FileReader fr(file_name);

	::mpl::Parser parser(fr);

	parser.parse();
}

#endif