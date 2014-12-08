#include "slr_parser.h"

#include <iostream>
#include <stack>

#include "detail/slr_grammar.h"

namespace mpl {
namespace parser {

	
static ::mpl::parser::detail::SLRGrammar s_grammar;
static bool s_init = false;

static const std::vector<std::pair<std::string, std::string> > s_rules = {
	// SLR
	{ "s", "e" },
	{ "e", "t" },
	{ "e", "e '+' t" },
	{ "t", "NUMBER" },
	{ "t", "'(' e ')'" },

	// LR1
	/*
	{ "s", "e" },
	{ "e", "l '=' r" },
	{ "e", "r" },
	{ "l", "ID" },
	{ "l", "'*' ID" },
	{ "r", "l" },
	*/
};

static void init() {
	for (size_t i = 0; i < s_rules.size(); i++) {
		s_grammar.add(s_rules[i].first, s_rules[i].second);
	}

	s_grammar.build();

	s_grammar.debug();
}

SLRParser::SLRParser(::mpl::Reader& reader) :
		_lexer(reader) {
	if (!s_init) {
		init();
		s_init = true;
	}
}

SLRParser::~SLRParser() {

}

void SLRParser::parse() {
	typedef ::mpl::parser::detail::SLRGrammar SLRGrammar;

	std::stack<size_t> st;
	st.push(0);


	int token = TokenType::EPSILON;

	// 有lookahead的EOS处理貌似都有问题,需要考虑下
	// 应该是缺少 accept状态所致
	while (!st.empty()) {
		size_t state = st.top();

		std::cout << "state: " << state << "\t";

		const SLRGrammar::Tran& tran = s_grammar[state];
		
		bool should_next = false;
		if (token == TokenType::EPSILON) {
			Token lookahead = _lexer.lookahead();
			token = lookahead.type;
			should_next = true;
		}

		std::cout << "token: " << s_grammar.name(token) << "\t";

		SLRGrammar::Tran::const_iterator it = tran.find(token);
		assert(it != tran.end());
		if (it->second.first < 0) {
			int left = it->second.first;
			int rule_no = it->second.second;

			const SLRGrammar::InnerRule& rule = s_grammar.rule(left, rule_no);
			for (size_t i = 0; i < rule.size(); i++) {
				st.pop();
			}
			token = left;
			std::cout << "reduce: " << s_grammar.name(left) << std::endl;
		} else if (it->second.first == s_grammar.ACCEPT) {
			std::cout << "accept" << std::endl;
			break;
		} else {
			st.push(it->second.second);
			std::cout << "shift: " << it->second.second << std::endl;

			token = TokenType::EPSILON;
			if (should_next) {
				_lexer.next();
			}
		}
	}
}
	
} // namespace parser
} // namespace mpl

#if 0

#include "../file_reader.h"
#include "../string_reader.h"
//#include "GeneratedLR1Parser.h"

using namespace std;

int main() {
	// ::mpl::StringReader sr("a = * a");
	::mpl::StringReader sr("2 + (2 + 2 + 2)");

	//::mpl::parser::GeneratedLR0Parser parser(sr);
	::mpl::parser::SLRParser parser(sr);

	parser.parse();

	return 0;
}

#endif