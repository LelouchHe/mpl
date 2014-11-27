#include "gramma.h"

#include <iostream>
#include <cassert>
#include <queue>

namespace mpl {
namespace parser {

template <typename Token>
Gramma<Token>::Gramma() {

}

template <typename Token>
Gramma<Token>::~Gramma() {

}

template <typename Token>
void Gramma<Token>::debug() {
	for (std::map<Token, Rules>::const_iterator it = _gramma.begin();
			it != _gramma.end(); ++it) {
		const Token& left = it->first;
		const Rules& rules = it->second;

		for (size_t i = 0; i < rules.size(); i++) {
			std::cout << left.text << " : ";
			for (size_t j = 0; j < rules[i].size(); j++) {
				std::cout << rules[i][j].text << " ";
			}
			std::cout << std::endl;
		}
	}

	std::cout << "nullable" << std::endl;
	for (std::map<Token, bool>::const_iterator it = _nullable.begin();
			it != _nullable.end(); ++it) {
		std::cout << it->first.text << " : " << it->second << std::endl;
	}

	std::cout << "first" << std::endl;
	for (std::map<Token, Tokens>::const_iterator it = _first.begin();
			it != _first.end(); ++it) {
		std::cout << it->first.text << " : ";
		for (Tokens::iterator tit = it->second.begin(); tit != it->second.end(); ++tit) {
			std::cout << tit->text << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "follow" << std::endl;
	for (std::map<Token, Tokens>::const_iterator it = _follow.begin();
			it != _follow.end(); ++it) {
		std::cout << it->first.text << " : ";
		for (Tokens::iterator tit = it->second.begin(); tit != it->second.end(); ++tit) {
			std::cout << tit->text << " ";
		}
		std::cout << std::endl;
	}
}

template <typename Token>
void Gramma<Token>::add(const Token& token, const Rule& rule) {
	assert(token.type == TokenType::NONTERMINAL);
	if (_start.type == TokenType::EOS) {
		_start = token;
	}
	_gramma[token].push_back(rule);
}

template <typename Token>
bool Gramma<Token>::build() {

	debug();

	std::cout << " ---- " << std::endl;

	if (!dedup()) {
		return false;
	}

	debug();

	std::cout << " ---- " << std::endl;

	if (!left_recursion()) {
		return false;
	}

	debug();

	std::cout << " ---- " << std::endl;

	if (!left_factor()) {
		return false;
	}

	debug();

	std::cout << " ---- " << std::endl;

	add_fake_start();

	debug();

	std::cout << " ---- " << std::endl;

	generate_nullable();

	generate_first();

	generate_follow();

	debug();

	std::cout << " ---- " << std::endl;

	return true;
}

template <typename Token>
bool Gramma<Token>::dedup() {
	std::map<Token, Rules> new_gramma;
	for (std::map<Token, Rules>::const_iterator it = _gramma.begin();
			it != _gramma.end(); ++it) {
		const Token& left = it->first;
		const Rules& rules = it->second;
		std::set<Rule> s;
		for (size_t i = 0; i < rules.size(); i++) {
			s.insert(rules[i]);
		}
		new_gramma[left].insert(new_gramma[left].end(), s.begin(), s.end());
	}

	_gramma.swap(new_gramma);

	return true;
}

template <typename Token>
bool Gramma<Token>::left_recursion() {
	std::map<Token, Rules> new_gramma;
	for (std::map<Token, Rules>::const_iterator it = _gramma.begin();
			it != _gramma.end(); ++it) {
		const Token& left = it->first;
		Rules rules = it->second;
		Rules new_rules;
		for (size_t i = 0; i < rules.size(); i++) {
			Rule rule = rules[i];
			if (rule.empty()) {
				continue;
			}

			const Token& right = rule[0];
			if (right.type != TokenType::NONTERMINAL || left <= right) {
				new_rules.push_back(rule);
				continue;
			}

			// 由于left>right,所以此时new_gramma肯定有值
			std::map<Token, Rules>::const_iterator rit = new_gramma.find(right);
			assert(rit != new_gramma.end());
			const Rules& right_rules = rit->second;
			for (size_t j = 0; j < right_rules.size(); j++) {
				Rule new_rule(right_rules[j]);
				new_rule.insert(new_rule.end(), rule.begin() + 1, rule.end());

				// 由于rules类似queue的作用,所以不能为const,所以rule也不能为const &,只能复制
				// 因为vector可能重新分配内存
				rules.push_back(new_rule);
			}
		}

		Rules recursion;
		Rules non_recursion;
		for (size_t i = 0; i < new_rules.size(); i++) {
			const Rule& rule = new_rules[i];
			if (rule.empty()) {
				continue;
			}

			if (rule[0] == left) {
				Rule new_rule(rule.begin() + 1, rule.end());
				recursion.push_back(new_rule);
			} else {
				non_recursion.push_back(rule);
			}
		}

		new_rules.clear();
		if (recursion.empty()) {
			new_rules.insert(new_rules.end(), non_recursion.begin(), non_recursion.end());
			new_gramma[left] = new_rules;
		} else {
			Token new_left(left.type, left.text + "*");

			for (size_t i = 0; i < non_recursion.size(); i++) {
				non_recursion[i].push_back(new_left);
				new_gramma[left].push_back(non_recursion[i]);
			}

			for (size_t i = 0; i < recursion.size(); i++) {
				recursion[i].push_back(new_left);
				new_gramma[new_left].push_back(recursion[i]);
			}
			new_gramma[new_left].push_back(Rule());
		}
	}

	_gramma.swap(new_gramma);

	return true;
}

template <typename Token>
bool Gramma<Token>::left_factor() {
	std::map<Token, Rules> new_gramma;
	for (std::map<Token, Rules>::const_iterator it = _gramma.begin();
			it != _gramma.end(); ++it) {
		std::map<Token, Rules> g;
		std::string suffix;
		left_factor_token(it->first, it->second, &suffix, &g);
		new_gramma.insert(g.begin(), g.end());
	}

	_gramma.swap(new_gramma);
	return true;
}

template <typename Token>
bool Gramma<Token>::left_factor_token(
		const Token& token, 
		const Rules& rules,
		std::string* suffix,
		std::map<Token, Rules>* g) {
	std::map<Token, Rules> groups;
	size_t size = rules.size();
	for (size_t i = 0; i < size; i++) {
		const Rule& rule = rules[i];
		if (rule.empty()) {
			Token t(TokenType::EPSILON, "");
			groups[t].push_back(rule);
		} else {
			groups[rule[0]].push_back(Rule(rule.begin() + 1, rule.end()));
		}
	}

	for (std::map<Token, Rules>::const_iterator it = groups.begin();
			it != groups.end(); ++it) {
		const Token& right = it->first;
		const Rules& right_rules = it->second;
		if (right_rules.size() == 1) {
			if (right.type == TokenType::EPSILON) {
				(*g)[token].push_back(Rule());
			} else {
				Rule rule;
				rule.push_back(right);
				rule.insert(rule.end(), right_rules[0].begin(), right_rules[0].end());
				(*g)[token].push_back(rule);
			}
			continue;
		}

		suffix->push_back('\'');
		Token new_token(token.type, token.text + *suffix);
		Rule rule;
		rule.push_back(right);
		rule.push_back(new_token);
		(*g)[token].push_back(rule);

		std::map<Token, Rules> new_g;
		left_factor_token(new_token, right_rules, suffix, &new_g);
		g->insert(new_g.begin(), new_g.end());
	}

	return true;
}

template <typename Token>
void Gramma<Token>::merge(const std::map<Token, Rules>& g, bool append) {
	for (std::map<Token, Rules>::const_iterator it = g.begin();
			it != g.end(); ++it) {
		if (append) {
			_gramma[it->first].insert(_gramma[it->first].end(), it->second.begin(), it->second.end());
		} else {
			_gramma[it->first] = it->second;
		}
	}
}

template <typename Token>
void Gramma<Token>::add_fake_start() {
	Token new_start(_start.type, "*" + _start.text);
	Rule rule;
	rule.push_back(_start);
	rule.push_back(Token(TokenType::EOS, "$"));
	add(new_start, rule);

	_start = new_start;
}

#if 0

nullable("") = true
nullable(T)  = false
nullable(a b) = nullable(a) && nullable(b)
nullable(a | b) = nullable(a) || nullable(b)

#endif

template <typename Token>
bool Gramma<Token>::generate_nullable() {
	for (std::map<Token, Rules>::const_iterator it = _gramma.begin();
			it != _gramma.end(); ++it) {
		assert(it->first.type == TokenType::NONTERMINAL);
		_nullable[it->first] = false;
	}

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (std::map<Token, bool>::iterator it = _nullable.begin();
				it != _nullable.end(); ++it) {
			if (it->second) {
				continue;
			}

			const Rules& rules = _gramma[it->first];
			for (size_t i = 0; i < rules.size(); i++) {
				const Rule& rule = rules[i];
				if (rule.empty()) {
					it->second = true;
					is_changing = true;
					break;
				}

				bool is_nullable = true;
				for (size_t j = 0; j < rule.size(); j++) {
					if (rule[j].type != TokenType::NONTERMINAL || !_nullable[rule[j]]) {
						is_nullable = false;
						break;
					}
				}

				if (is_nullable) {
					it->second = true;
					is_changing = true;
					break;
				}
			}
		}
	}



	return true;
}

#if 0

first("") = null
first(T)  = T
first(a b) = first(a)
			 first(a) + first(b)  nullable(a) = true
first(a | b) = first(a) + first(b)

#endif

template <typename Token>
bool Gramma<Token>::generate_first() {
	// 可以加一个bool位标,表示是否变动,作为优化
	for (std::map<Token, Rules>::const_iterator it = _gramma.begin();
			it != _gramma.end(); ++it) {
		assert(it->first.type == TokenType::NONTERMINAL);
		_first[it->first].clear();
	}

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;
		for (std::map<Token, Tokens>::iterator it = _first.begin();
				it != _first.end(); ++it) {
			const Token& token = it->first;
			Tokens& tokens = it->second;

			const Rules& rules = _gramma[token];
			for (size_t i = 0; i < rules.size(); i++) {
				const Rule& rule = rules[i];
				if (rule.empty()) {
					continue;
				}

				std::pair<Tokens::iterator, bool> ret;
				if (rule[0].type != TokenType::NONTERMINAL) {
					ret = tokens.insert(rule[0]);
					if (ret.second) {
						is_changing = true;
					}
					continue;
				}

				bool is_nullable = true;
				for (size_t j = 0; j < rule.size() && is_nullable; j++) {
					const Tokens& right_tokens = _first[rule[j]];
					for (Tokens::const_iterator tit = right_tokens.begin();
							tit != right_tokens.end(); ++tit) {
						ret = tokens.insert(*tit);
						if (ret.second) {
							is_changing = true;
						}
					}
					
					if (rule[j].type != TokenType::NONTERMINAL) {
						is_nullable = false;
					} else {
						is_nullable = _nullable[rule[j]];
					}
				}
			}
		}
	}

	return true;
}

template <typename Token>
bool Gramma<Token>::generate_follow() {
	return true;
}

template <typename Token>
const typename Gramma<Token>::Rule& Gramma<Token>::fetch(const Token& token, const Token& next) {

}

} // namespace parser
} // namespace mpl

#if 1


#include "../lexer/token.h"

using namespace std;

int main() {
	::mpl::parser::Gramma<::mpl::lexer::Token> gramma;
	::mpl::parser::Gramma<::mpl::lexer::Token>::Rule rule;

	::mpl::lexer::Token goal(::mpl::lexer::Token::TokenType::NONTERMINAL, "goal");
	::mpl::lexer::Token expr(::mpl::lexer::Token::TokenType::NONTERMINAL, "expr");
	::mpl::lexer::Token term(::mpl::lexer::Token::TokenType::NONTERMINAL, "term");
	::mpl::lexer::Token factor(::mpl::lexer::Token::TokenType::NONTERMINAL, "factor");
	::mpl::lexer::Token plus(::mpl::lexer::Token::TokenType::TT_PLUS, "+");
	::mpl::lexer::Token minus(::mpl::lexer::Token::TokenType::TT_MINUS, "-");
	::mpl::lexer::Token mul(::mpl::lexer::Token::TokenType::TT_MUL, "*");
	::mpl::lexer::Token div(::mpl::lexer::Token::TokenType::TT_DIV, "/");
	::mpl::lexer::Token lp(::mpl::lexer::Token::TokenType::TT_LEFT_PARENTHESIS, "(");
	::mpl::lexer::Token rp(::mpl::lexer::Token::TokenType::TT_RIGHT_PARENTHESIS, ")");
	::mpl::lexer::Token id(::mpl::lexer::Token::TokenType::TT_ID, "ID");
	::mpl::lexer::Token num(::mpl::lexer::Token::TokenType::TT_NUMBER, "NUM");

	rule.clear();
	rule.push_back(expr);
	gramma.add(goal, rule);

	rule.clear();
	rule.push_back(expr);
	rule.push_back(plus);
	rule.push_back(term);
	gramma.add(expr, rule);

	rule.clear();
	rule.push_back(expr);
	rule.push_back(minus);
	rule.push_back(term);
	gramma.add(expr, rule);

	rule.clear();
	rule.push_back(term);
	gramma.add(expr, rule);

	rule.clear();
	rule.push_back(term);
	rule.push_back(mul);
	rule.push_back(factor);
	gramma.add(term, rule);

	rule.clear();
	rule.push_back(term);
	rule.push_back(div);
	rule.push_back(factor);
	gramma.add(term, rule);

	rule.clear();
	rule.push_back(factor);
	gramma.add(term, rule);


	rule.clear();
	rule.push_back(lp);
	rule.push_back(expr);
	rule.push_back(rp);
	gramma.add(factor, rule);

	rule.clear();
	rule.push_back(id);
	gramma.add(factor, rule);

	rule.clear();
	rule.push_back(num);
	gramma.add(factor, rule);

	gramma.build();

	return 0;
}

#endif