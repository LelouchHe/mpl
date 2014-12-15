#ifndef MPL_PARSER_LR_PARSER_H
#define MPL_PARSER_LR_PARSER_H

#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <cassert>

#include "../lexer.h"
#include "../reader.h"
#include "../ast/parser_node.h"

namespace mpl {
namespace parser {

extern const std::vector<std::pair<std::string, std::string> > GRAMMAR_RULES;

template <typename Grammar>
class LRParser {
public:
	LRParser(::mpl::Reader& reader) :
			_lexer(reader) {
		if (s_init) {
			return;
		}

		for (size_t i = 0; i < GRAMMAR_RULES.size(); i++) {
			s_grammar.add(GRAMMAR_RULES[i].first, GRAMMAR_RULES[i].second);
		}

		s_grammar.add("'+'", 1, Grammar::LEFT);
		s_grammar.add("'*'", 2, Grammar::LEFT);

		s_grammar.build();
		s_grammar.debug();

		s_init = true;
	}

	~LRParser() {

	}

public:
	typedef ::mpl::Lexer Lexer;

	typedef Lexer::Token Token;
	typedef Token::TokenType TokenType;

	// ¼´½«·ÏÆú
	void parse() {
		std::stack<size_t> st;
		st.push(0);

		int token = TokenType::EPSILON;
		while (!st.empty()) {
			size_t state = st.top();

			std::cout << "state: " << state << "\t";

			const typename Grammar::Tran& tran = s_grammar[state];

			bool should_next = false;
			if (token == TokenType::EPSILON) {
				Token lookahead = _lexer.lookahead();
				token = lookahead.type;
				should_next = true;
			}

			std::cout << "token: " << s_grammar.name(token) << "\t";

			typename Grammar::Tran::const_iterator it = tran.find(token);
			assert(it != tran.end());
			if (it->second.first < 0) {
				int left = it->second.first;
				int rule_no = it->second.second;

				const typename Grammar::InnerRule& rule = s_grammar.rule(left, rule_no);
				for (size_t i = 0; i < rule.size(); i++) {
					st.pop();
				}

				token = left;
				std::cout << "reduce: " << s_grammar.name(token) << std::endl;
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

	::mpl::ast::ParserNodePtr build_parser_tree() {
		std::stack<size_t> st;
		std::vector< ::mpl::ast::ParserNodePtr> nodes;
		st.push(0);

		Token token(TokenType::EPSILON, "");
		while (!st.empty()) {
			size_t state = st.top();

			std::cout << "state: " << state << "\t";

			const typename Grammar::Tran& tran = s_grammar[state];

			bool should_next = false;
			if (token.type == TokenType::EPSILON) {
				token = _lexer.lookahead();
				should_next = true;
			}

			std::cout << "token: " << token.text << "\t";

			typename Grammar::Tran::const_iterator it = tran.find(token.type);
			assert(it != tran.end());
			if (it->second.first < 0) {
				int left = it->second.first;
				int rule_no = it->second.second;

				token.type = (TokenType)left;
				token.text = s_grammar.name(left);
				std::cout << "reduce: " << token.text << std::endl;

				::mpl::ast::ParserNodePtr parent = ::mpl::ast::ParserNode::create(token);

				const typename Grammar::InnerRule& rule = s_grammar.rule(left, rule_no);
				size_t children_start = nodes.size() - rule.size();
				for (size_t i = 0; i < rule.size(); i++) {
					st.pop();
					parent->add(nodes[children_start + i]);
				}
				nodes.resize(children_start);
				nodes.push_back(parent);
			} else if (it->second.first == s_grammar.ACCEPT) {
				std::cout << "accept" << std::endl;
				break;
			} else {
				st.push(it->second.second);
				std::cout << "shift: " << it->second.second << std::endl;

				if (token.type >= 0) {
					nodes.push_back(::mpl::ast::ParserNode::create(token));
				}

				token.type = TokenType::EPSILON;
				token.text = "";
				if (should_next) {
					_lexer.next();
				}
			}
		}

		if (nodes.size() == 1) {
			return nodes[0];
		} else {
			return ::mpl::ast::ParserNodePtr();
		}
	}

private:
	Lexer _lexer;

	static Grammar s_grammar;
	static bool s_init;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_LR_PARSER_H