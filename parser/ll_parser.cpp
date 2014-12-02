#include "ll_parser.h"

#include <map>
#include <stack>
#include <iostream>
#include <cassert>

#include "../reader.h"
#include "../string_reader.h"
#include "gramma.h"

namespace mpl {
namespace parser {

static Gramma s_gramma;
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

// Ç°ºó×·¸Ï
static std::vector<std::string> split(const std::string& str, char delim) {
	std::vector<std::string> strs;

	size_t begin = 0;
	size_t end = begin;
	while (end < str.size()) {
		if (str[end] == delim) {
			while (begin < end && str[begin] == delim) {
				begin++;
			}
			if (begin != end) {
				strs.push_back(str.substr(begin, end - begin));
				begin = end;
			}
		}
		end++;
	}
	while (begin < end && str[begin] == delim) {
		begin++;
	}
	if (begin != end) {
		strs.push_back(str.substr(begin, end - begin));
	}

	return strs;
}

static void init() {
	for (size_t i = 0; i < s_rules.size(); i++) {
		Gramma::Token left(Gramma::TokenType::NONTERMINAL, s_rules[i].first);

		Gramma::Rule rule;
		std::vector<std::string> strs = split(s_rules[i].second, ' ');
		for (size_t j = 0; j < strs.size(); j++) {
			const std::string& str = strs[j];
			if (str.empty()) {
				continue;
			}

			Gramma::Token token;
			if (str[0] == '\'') {
				assert(str[str.size() - 1] == '\'');

				::mpl::StringReader reader(str.substr(1, str.size() - 2));
				LLParser::Lexer lexer(reader);
				token = lexer.next();
				assert(lexer.lookahead().type == Gramma::TokenType::EOS);
			} else {
				Gramma::TokenType type = Lexer::token_type(str);
				if (type != Gramma::TokenType::ERROR) {
					token.type = type;	
				} else {
					token.type = Gramma::TokenType::NONTERMINAL;
				}
				token.text = str;
			}

			rule.push_back(token);
		}

		s_gramma.add(left, rule);
	}

	s_gramma.build();
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
	std::stack<int> st;
	st.push(s_gramma.start());

	Token current = _lexer.next();
	while (!st.empty() && current.type != TokenType::ERROR) {
		int token = st.top();
		st.pop();
		std::cout << "expected: " << s_gramma.name(token) << std::endl;
		if (token >= 0) {
			if ((TokenType)token == current.type) {
				std::cout << "match: (" << current.type << ", " << current.text << ")" << std::endl;
				current = _lexer.next();
			} else {
				std::cout << "not match: (" << current.type << ", " << current.text << ")" << std::endl;
				break;
			}
		} else {
			const Gramma::Tran& tran = s_gramma[token];
			Gramma::Tran::const_iterator it = tran.find(current.type);
			if (it == tran.end()) {
				std::cout << "not match" << std::endl;
			}

			const Gramma::InnerRule& rule = s_gramma.rule(token, it->second);
			if (rule.empty()) {
				std::cout << "match empty" << std::endl;
				continue;
			}

			for (int i = rule.size() - 1; i >= 0; i--) {
				st.push(rule[i]);
			}
		}
	}
}

} // namespace parser
} // namespace mpl

#if 1

#include "../file_reader.h"
#include "../string_reader.h"

using namespace std;

int main() {
	::mpl::FileReader fr("parser.txt");
	::mpl::StringReader sr("2+3*4");

	::mpl::parser::LLParser parser(sr);

	parser.parse();

	return 0;
}

#endif