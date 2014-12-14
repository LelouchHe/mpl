#include "ll_parser.h"

#include <map>
#include <stack>
#include <iostream>
#include <cassert>

#include "../reader.h"
#include "../string_reader.h"
#include "grammar/ll_grammar.h"

namespace mpl {
namespace parser {

static ::mpl::parser::grammar::LLGrammar s_grammar;
static bool s_init = false;

static const std::vector<std::pair<std::string, std::string> > s_rules = {
	{ "goal", "expr" },
	{ "expr", "expr '+' term" },
	{ "expr", "expr '-' term" },
	{ "expr", "term" },
	{ "term", "term '*' factor" },
	{ "term", "term '/' factor" },
	{ "term", "factor" },
	{ "factor", "'(' expr ')'" },
	{ "factor", "NUMBER" },
	{ "factor", "ID" },

	/*
	{ "T", "R" },
	{ "T", "ID T 'c'" },
	{ "R", "" },
	{ "R", "'b' R" },
	*/
};

static void init() {
	for (size_t i = 0; i < s_rules.size(); i++) {
		s_grammar.add(s_rules[i].first, s_rules[i].second);
	}

	s_grammar.build();
}

LLParser::LLParser(::mpl::Reader& reader) :
		_lexer(reader) {
	if (!s_init) {
		init();
		s_init = true;
	}
}

LLParser::~LLParser() {

}

void LLParser::parse() {
	typedef ::mpl::parser::grammar::LLGrammar LLGrammar;

	std::stack<int> st;
	st.push(s_grammar.start());

	Token current = _lexer.next();
	while (!st.empty() && current.type != TokenType::ERROR) {
		int token = st.top();
		st.pop();
		std::cout << "expected: " << s_grammar.name(token) << std::endl;
		if (token >= 0) {
			if ((TokenType)token == current.type) {
				std::cout << "match: (" << current.type << ", " << current.text << ")" << std::endl;
				current = _lexer.next();
			} else {
				std::cout << "not match: (" << current.type << ", " << current.text << ")" << std::endl;
				break;
			}
		} else {
			const LLGrammar::Tran& tran = s_grammar[token];
			LLGrammar::Tran::const_iterator it = tran.find(current.type);
			if (it == tran.end()) {
				std::cout << "not match" << std::endl;
				break;
			}

			const LLGrammar::InnerRule& rule = s_grammar.rule(token, it->second);
			if (rule.empty()) {
				std::cout << "match empty" << std::endl;
				continue;
			}

			for (LLGrammar::InnerRule::const_reverse_iterator rit = rule.rbegin();
					rit != rule.rend(); ++rit) {
				st.push(*rit);
			}
		}
	}
}

} // namespace parser
} // namespace mpl

#if 0

#include "../file_reader.h"
#include "../string_reader.h"
#include "GeneratedLLParser.h"

using namespace std;

int main() {
	::mpl::FileReader fr("parser.txt");
	::mpl::StringReader sr("2+3*4");

	//::mpl::parser::GeneratedLLParser parser(sr);
	::mpl::parser::LLParser parser(sr);


	parser.parse();

	return 0;
}

#endif