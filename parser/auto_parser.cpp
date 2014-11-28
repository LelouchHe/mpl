#include "auto_parser.h"

#include <stack>
#include <iostream>
#include "../reader.h"

namespace mpl {
namespace parser {

template <typename Lexer>
AutoParser<Lexer>::AutoParser(::mpl::Reader& reader) :
		_lexer(reader) {
	init();
}

template <typename Lexer>
AutoParser<Lexer>::~AutoParser() {

}

template <typename Lexer>
void AutoParser<Lexer>::init() {
	Gramma<Token>::Rule rule;
	Token T(TokenType::NONTERMINAL, "T");
	Token R(TokenType::NONTERMINAL, "R");
	Token a(TokenType::TT_ID, "a");
	Token b(TokenType::TT_ID, "b");
	Token c(TokenType::TT_ID, "c");

	rule.clear();
	rule.push_back(R);
	_gramma.add(T, rule);

	rule.clear();
	rule.push_back(a);
	rule.push_back(T);
	rule.push_back(c);
	_gramma.add(T, rule);

	rule.clear();
	_gramma.add(R, rule);

	rule.clear();
	rule.push_back(b);
	rule.push_back(R);
	_gramma.add(R, rule);

	_gramma.build();
}

template <typename Lexer>
void AutoParser<Lexer>::parse() {
	std::stack<Token> st;
	st.push(_gramma.start());

	while (!st.empty() && _lexer.lookahead().type != TokenType::ERROR) {
		Token token = st.top();
		st.pop();

		const Token& current = _lexer.next();

		std::cout << "expected: " << token.text << std::endl;

		if (token.type != TokenType::NONTERMINAL) {
			if (token == current) {
				std::cout << "match" << std::endl;
			} else {
				std::cout << "not match" << std::endl;
				std::cout << "actually: " << current.text << std::endl;
				break;
			}
		} else {
			Gramma<Token>::Rule new_rule = _gramma.fetch(token, current);
			if (new_rule.empty()) {
				std::cout << "match empty" << std::endl;
				continue;
			}

			for (int i = new_rule.size() - 1; i >= 0; i--) {
				st.push(new_rule[i]);
			}
		}
	}
}

} // namespace parser
} // namespace mpl

#if 1

#include "../lexer/GeneratedLexer.h"
#include "../file_reader.h"

using namespace std;

int main() {
	const char* file_name = "parser.txt";

	::mpl::FileReader fr(file_name);

	::mpl::parser::AutoParser<::mpl::lexer::GeneratedLexer> parser(fr);

	parser.parse();

	return 0;
}

#endif