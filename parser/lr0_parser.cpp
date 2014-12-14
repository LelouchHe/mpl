#include "lr0_parser.h"

#include <iostream>
#include <stack>

#include "grammar/lr0_grammar.h"

namespace mpl {
namespace parser {

	
static ::mpl::parser::grammar::LR0Grammar s_grammar;
static bool s_init = false;

static const std::vector<std::pair<std::string, std::string> > s_rules = {
	{ "s", "e" },
	{ "e", "t ';'" },
	{ "e", "t '+' e" },
	{ "t", "NUMBER" },
	{ "t", "'(' e ')'" },

	// LR1
	/*
	{ "s", "e" },
	{ "e", "t" },
	{ "e", "e '+' t" },
	{ "t", "NUMBER" },
	{ "t", "'(' e ')'" },
	*/
};

static void init() {
	for (size_t i = 0; i < s_rules.size(); i++) {
		s_grammar.add(s_rules[i].first, s_rules[i].second);
	}

	s_grammar.build();
}

LR0Parser::LR0Parser(::mpl::Reader& reader) :
		_lexer(reader) {
	if (!s_init) {
		init();
		s_init = true;
	}
}

LR0Parser::~LR0Parser() {

}

void LR0Parser::parse() {
	typedef ::mpl::parser::grammar::LR0Grammar LR0Grammar;

	std::stack<size_t> st;
	st.push(0);


	int token = TokenType::EPSILON;
	while (!st.empty()) {
		size_t state = st.top();

		std::cout << "state: " << state << "\taction: ";

		const LR0Grammar::Tran& tran = s_grammar[state];
		LR0Grammar::Tran::const_iterator it = tran.find(TokenType::EPSILON);
		if (it != tran.end()) {
			assert(tran.size() == 1);
			int left = it->second.first;
			int rule_no = it->second.second;

			const LR0Grammar::InnerRule& rule = s_grammar.rule(left, rule_no);
			for (size_t i = 0; i < rule.size(); i++) {
				st.pop();
			}
			token = left;
			std::cout << "reduce (" << s_grammar.name(left) << ")" << std::endl;
		} else {
			if (token == TokenType::EPSILON) {
				Token current = _lexer.next();
				token = current.type;
			}

			it = tran.find(token);
			assert(it != tran.end());
			assert(it->second.first >= 0);

			if (it->second.first == s_grammar.ACCEPT) {
				std::cout << "accept" << std::endl;
				break;
			}

			st.push(it->second.second);
			std::cout << "shift (" << it->second.second << ")" << std::endl;

			token = TokenType::EPSILON;
		}
	}
}
	
} // namespace parser
} // namespace mpl

#if 0

#include "../file_reader.h"
#include "../string_reader.h"
//#include "GeneratedLR0Parser.h"

using namespace std;

int main() {
	::mpl::StringReader sr("2 + (2 + 2;);");

	//::mpl::parser::GeneratedLR0Parser parser(sr);
	::mpl::parser::LR0Parser parser(sr);

	parser.parse();

	return 0;
}

#endif