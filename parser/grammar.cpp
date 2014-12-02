#include "gramma.h"

#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>

namespace mpl {
namespace parser {

const Gramma::Rule Gramma::NON_RULE;

static const size_t NONTERMINAL_START = 1;

static inline int token2index(int token) {
	assert(token < 0);

	return -token;
}

static inline int index2token(int index) {
	assert(index > 0);
	return -index;
}

Gramma::Gramma() : _start(0) {
	// 0专门空出
	_nonterminals.push_back("");
	_rules.push_back(InnerRules());
}

Gramma::~Gramma() {

}

void Gramma::debug() {
	for (size_t i = NONTERMINAL_START; i < _rules.size(); i++) {
		const InnerRules& rules = _rules[i];
		for (size_t j = 0; j < rules.size(); j++) {
			const InnerRule& rule = rules[j];
			std::cout << _nonterminals[i] << " : ";
			for (size_t k = 0; k < rule.size(); k++) {
					std::cout << name(rule[k]) << " ";
			}
			std::cout << std::endl;
		}
	}

	std::cout << " --nullable--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _nullable.size(); i++) {
		std::cout << _nonterminals[i] << " : " << _nullable[i] << std::endl;
	}

	std::cout << " --first--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _first.size(); i++) {
		std::cout << _nonterminals[i] << " : ";
		const Tokens& tokens = _first[i];
		for (Tokens::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
			std::cout << Lexer::token_name((TokenType)*it) << " ";
		}
		std::cout << std::endl;
	}

	std::cout << " --follow--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _follow.size(); i++) {
		std::cout << _nonterminals[i] << " : ";
		const Tokens& tokens = _follow[i];
		for (Tokens::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
			std::cout << Lexer::token_name((TokenType)*it) << " ";
		}
		std::cout << std::endl;
	}

	std::cout << " --trans--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _trans.size(); i++) {
		const Tran& tran = _trans[i];
		for (Tran::const_iterator it = tran.begin(); it != tran.end(); ++it) {
			std::cout << _nonterminals[i];
			std::cout << "(" << name(it->first) << ") -> " << it->second << std::endl;
		}
	}
}

size_t Gramma::new_nonternimal(const std::string& name) {
	assert(_nonterminals.size() == _rules.size());

	std::vector<std::string>::iterator it = std::find(_nonterminals.begin(), _nonterminals.end(), name);
	if (it != _nonterminals.end()) {
		return it - _nonterminals.begin();
	} else {
		size_t size = _nonterminals.size();

		_nonterminals.push_back(name);
		_rules.push_back(InnerRules());

		return size;
	}
}

void Gramma::add(const Token& token, const Rule& rule) {
	assert(token.type == TokenType::NONTERMINAL);
	int left = new_nonternimal(token.text);
	if (_start == 0) {
		_start = index2token(left);
	}

	InnerRule inner_rule;
	for (Rule::const_iterator it = rule.begin(); it != rule.end(); ++it) {
		const Token& right = *it;
		if (right.type != TokenType::NONTERMINAL) {
			inner_rule.push_back((int)right.type);
		} else {
			int index = new_nonternimal(right.text);
			inner_rule.push_back(index2token(index));
		}
	}

	_rules[left].push_back(inner_rule);
}

size_t Gramma::size() const {
	return _nonterminals.size();
}

const Gramma::Tran& Gramma::operator[](int token) const {
	token = token2index(token);
	assert(token > 0 && (size_t)token < _nonterminals.size());

	return _trans[token];
}
const std::string& Gramma::name(int token) const {
	if (token >= 0) {
		return Lexer::token_name((TokenType)token);
	} else {
		token = token2index(token);
		assert(token > 0 && (size_t)token < _nonterminals.size());
		return _nonterminals[token];
	}
}
const Gramma::InnerRule& Gramma::rule(int token, size_t index) const {
	token = token2index(token);
	assert(token > 0 && (size_t)token < _nonterminals.size());

	const InnerRules& rules = _rules[token];
	assert(index < rules.size());

	return rules[index];
}

int Gramma::start() const {
	return _start;
}

bool Gramma::build(GrammaOption option) {

	std::cout << " ---- " << std::endl;
	debug();

	if (!dedup()) {
		return false;
	}

	if (option.left_recursion && !left_recursion()) {
		return false;
	}

	if (option.left_factor && !left_factor()) {
		return false;
	}

	add_fake_start();

	generate_nullable();
	generate_first();
	generate_follow();

	generate_trans();

	std::cout << " ---- " << std::endl;
	debug();

	std::cout << " ---- " << std::endl;

	return true;
}


bool Gramma::dedup() {
	size_t size = _rules.size();
	for (size_t i = NONTERMINAL_START; i < size; i++) {
		InnerRules& rules = _rules[i];
		std::set<InnerRule> s;
		for (size_t j = 0; j < rules.size(); j++) {
			s.insert(rules[j]);
		}
		rules.clear();
		rules.insert(rules.end(), s.begin(), s.end());
	}

	return true;
}

// 没有显示的提供EPSILON,而是以empty为标识
// 左递归消除会代码很多额外规则(因为进行了替换),应该可以对那些没有参与递归转换的规则,再转换回去
bool Gramma::left_recursion() {
	size_t size = _rules.size();
	for (size_t left = NONTERMINAL_START; left < size; left++) {
		// 此处不能const,因为会添加new_rule并进行迭代
		InnerRules rules = _rules[left];
		InnerRules new_rules;
		for (size_t i = 0; i < rules.size(); i++) {
			InnerRule rule = rules[i];
			if (rule.empty()) {
				new_rules.push_back(rule);
				continue;
			}

			int right = rule[0];
			if (right >= 0 || left <= (size_t)token2index(right)) {
				new_rules.push_back(rule);
				continue;
			}

			const InnerRules& right_rules = _rules[token2index(right)];
			for (size_t j = 0; j < right_rules.size(); j++) {
				InnerRule new_rule(right_rules[j]);
				new_rule.insert(new_rule.end(), rule.begin() + 1, rule.end());

				// rules相当于一个队列,需要完全遍历
				rules.push_back(new_rule);
			}
		}

		InnerRules recursion;
		InnerRules non_recursion;
		for (size_t i = 0; i < new_rules.size(); i++) {
			const InnerRule& rule = new_rules[i];
			if (rule.empty()) {
				non_recursion.push_back(rule);
				continue;
			}

			if (rule[0] < 0 && rule[0] == index2token(left)) {
				InnerRule new_rule(rule.begin() + 1, rule.end());
				recursion.push_back(new_rule);
			} else {
				non_recursion.push_back(rule);
			}
		}

		_rules[left].clear();
		if (recursion.empty()) {
			_rules[left].insert(_rules[left].end(), non_recursion.begin(), non_recursion.end());
		} else {
			int new_left = new_nonternimal(_nonterminals[left] + "*");

			for (size_t i = 0; i < non_recursion.size(); i++) {
				non_recursion[i].push_back(index2token(new_left));
				_rules[left].push_back(non_recursion[i]);
			}

			for (size_t i = 0; i < recursion.size(); i++) {
				recursion[i].push_back(index2token(new_left));
				_rules[new_left].push_back(recursion[i]);
			}
			_rules[new_left].push_back(InnerRule());
		}
	}

	return true;
}


bool Gramma::left_factor() {
	size_t size = _rules.size();
	for (size_t i = NONTERMINAL_START; i < size; i++) {
		left_factor_token(i);
	}

	return true;
}

bool Gramma::left_factor_token(size_t token) {
	std::map<int, InnerRules> groups;
	const InnerRules& rules = _rules[token];
	size_t size = rules.size();
	for (size_t i = 0; i < size; i++) {
		const InnerRule& rule = rules[i];
		if (rule.empty()) {
			groups[TokenType::EPSILON].push_back(rule);
		} else {
			groups[rule[0]].push_back(InnerRule(rule.begin() + 1, rule.end()));
		}
	}

	_rules[token].clear();
	for (std::map<int, InnerRules>::iterator it = groups.begin();
			it != groups.end(); ++it) {
		int right = it->first;
		InnerRules& right_rules = it->second;

		if (right_rules.size() == 1) {
			if (right == TokenType::EPSILON) {
				_rules[token].push_back(InnerRule());
			} else {
				InnerRule rule;
				rule.push_back(right);
				rule.insert(rule.end(), right_rules[0].begin(), right_rules[0].end());
				_rules[token].push_back(rule);
			}
			continue;
		}

		int new_token = new_nonternimal(_nonterminals[token] + "\'");
		InnerRule rule;
		rule.push_back(right);
		rule.push_back(index2token(new_token));
		_rules[token].push_back(rule);

		_rules[new_token].swap(right_rules);

		left_factor_token(new_token);
	}

	return true;
}


void Gramma::add_fake_start() {
	int new_start = new_nonternimal("*" + name(_start));
	InnerRule rule;
	rule.push_back(_start);
	rule.push_back(TokenType::EOS);
	
	_rules[new_start].push_back(rule);

	_start = index2token(new_start);
}

#if 0

nullable("") = true
nullable(T)  = false
nullable(a b) = nullable(a) && nullable(b)
nullable(a | b) = nullable(a) || nullable(b)

#endif

bool Gramma::generate_nullable() {
	size_t size = _nonterminals.size();
	_nullable.resize(size);

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (size_t left = NONTERMINAL_START; left < size; left++) {
			if (_nullable[left]) {
				continue;
			}

			const InnerRules& rules = _rules[left];
			for (size_t i = 0; i < rules.size(); i++) {
				const InnerRule& rule = rules[i];

				bool is_nullable = true;
				for (size_t j = 0; j < rule.size() && is_nullable; j++) {
					if (rule[j] >= 0 || !_nullable[token2index(rule[j])]) {
						is_nullable = false;
					}
				}

				if (is_nullable) {
					_nullable[left] = true;
					is_changing = true;
					break;
				}
			}
		}
	}

	_rule_nullable.resize(size);
	for (size_t left = NONTERMINAL_START; left < size; left++) {
		const InnerRules& rules = _rules[left];
		_rule_nullable[left].resize(rules.size());

		for (size_t i = 0; i < rules.size(); i++) {
			const InnerRule& rule = rules[i];

			bool is_nullable = true;
			for (size_t j = 0; j < rule.size() && is_nullable; j++) {
				if (rule[j] >= 0) {
					is_nullable = false;
				} else {
					is_nullable = _nullable[token2index(rule[j])];
				}
			}

			_rule_nullable[left][i] = is_nullable;
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

bool Gramma::generate_first() {
	// 可以加一个bool位标,表示是否变动,作为优化
	size_t size = _nonterminals.size();
	_first.resize(size);

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (size_t left = NONTERMINAL_START; left < size; left++) {
			Tokens& tokens = _first[left];

			const InnerRules& rules = _rules[left];
			for (size_t i = 0; i < rules.size(); i++) {
				const InnerRule& rule = rules[i];
				if (rule.empty()) {
					continue;
				}

				std::pair<Tokens::iterator, bool> ret;
				if (rule[0] >= 0) {
					ret = tokens.insert((TokenType)rule[0]);
					if (ret.second) {
						is_changing = true;
					}
					continue;
				}

				bool is_nullable = true;
				for (size_t j = 0; j < rule.size() && is_nullable; j++) {
					if (rule[j] >= 0) {
						ret = tokens.insert((TokenType)rule[j]);
						if (ret.second) {
							is_changing = true;
						}
						is_nullable = false;
					} else {
						const Tokens& right_tokens = _first[token2index(rule[j])];
						for (Tokens::const_iterator it = right_tokens.begin();
							it != right_tokens.end(); ++it) {
							ret = tokens.insert(*it);
							if (ret.second) {
								is_changing = true;
							}
						}
						is_nullable = _nullable[token2index(rule[j])];
					}
				}
			}
		}
	}

	_rule_first.resize(size);
	for (size_t left = NONTERMINAL_START; left < size; left++) {
		const InnerRules& rules = _rules[left];
		_rule_first[left].resize(rules.size());

		for (size_t i = 0; i < rules.size(); i++) {
			const InnerRule& rule = rules[i];

			bool is_nullable = true;
			for (size_t j = 0; j < rule.size() && is_nullable; j++) {
				if (rule[j] >= 0) {
					_rule_first[left][i].insert((TokenType)rule[j]);
					is_nullable = false;
				} else {
					const Tokens& first = _first[token2index(rule[j])];
					_rule_first[left][i].insert(first.begin(), first.end());
					is_nullable = _nullable[token2index(rule[j])];
				}
			}
		}
	}

	return true;
}

#if 0

a : b c
follow(b) = first(c)
			follow(a) nullable(c) = true

#endif

bool Gramma::generate_follow() {
	size_t size = _nonterminals.size();
	_follow.resize(size);

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (size_t left = NONTERMINAL_START; left < size; left++) {
			const InnerRules& rules = _rules[left];
			for (size_t i = 0; i < rules.size(); i++) {
				const InnerRule& rule = rules[i];
				if (rule.empty()) {
					continue;
				}

				std::pair<Tokens::iterator, bool> ret;
				bool is_nullable = true;
				for (int j = rule.size() - 1; j >= 0; j--) {
					if (rule[j] >= 0) {
						is_nullable = false;
						continue;
					}

					Tokens& tokens = _follow[token2index(rule[j])];

					if ((size_t)j < rule.size() - 1) {
						int next = rule[j + 1];
						if (next >= 0) {
							ret = tokens.insert((TokenType)next);
							if (ret.second) {
								is_changing = true;
							}
							is_nullable = false;
						} else {
							const Tokens& next_tokens = _first[token2index(next)];
							for (Tokens::const_iterator it = next_tokens.begin();
									it != next_tokens.end(); ++it) {
								ret = tokens.insert(*it);
							}
						}
					}

					if (is_nullable) {
						const Tokens& left_tokens = _follow[left];
						for (Tokens::const_iterator tit = left_tokens.begin();
								tit != left_tokens.end(); tit++) {
							ret = tokens.insert(*tit);
							if (ret.second) {
								is_changing = true;
							}
						}
					}
				}
			}
		}
	}

	return true;
}

#if 0

a ->(c) b

c in FIRST(b)
     FIRST(b) + FOLLOW(a)  nullable(b) = true

#endif	


bool Gramma::generate_trans() {
	size_t size = _nonterminals.size();
	_trans.resize(size);

	for (size_t left = NONTERMINAL_START; left < size; left++) {
		const InnerRules& rules = _rules[left];

		for (size_t i = 0; i < rules.size(); i++) {
			const InnerRule& rule = rules[i];
			const Tokens& right_first = _rule_first[left][i];
			for (Tokens::const_iterator it = right_first.begin();
					it != right_first.end(); ++it) {
				_trans[left][*it] = i;
			}

			if (_rule_nullable[left][i]) {
				const Tokens& follow = _follow[left];
				for (Tokens::const_iterator it = follow.begin();
						it != follow.end(); ++it) {
					_trans[left][*it] = i;
				}
			}
		}
	}
	
	return true;
}

} // namespace parser
} // namespace mpl

#if 0

#include "../lexer/token.h"
#include <stack>

using namespace std;

int main() {
	::mpl::parser::Gramma gramma;
	::mpl::parser::Gramma::Rule rule;

#if 1

	::mpl::Lexer::Token goal(::mpl::Lexer::Token::TokenType::NONTERMINAL, "goal");
	::mpl::Lexer::Token expr(::mpl::Lexer::Token::TokenType::NONTERMINAL, "expr");
	::mpl::Lexer::Token term(::mpl::Lexer::Token::TokenType::NONTERMINAL, "term");
	::mpl::Lexer::Token factor(::mpl::Lexer::Token::TokenType::NONTERMINAL, "factor");
	::mpl::Lexer::Token plus(::mpl::Lexer::Token::TokenType::TT_PLUS, "+");
	::mpl::Lexer::Token minus(::mpl::Lexer::Token::TokenType::TT_MINUS, "-");
	::mpl::Lexer::Token mul(::mpl::Lexer::Token::TokenType::TT_MUL, "*");
	::mpl::Lexer::Token div(::mpl::Lexer::Token::TokenType::TT_DIV, "/");
	::mpl::Lexer::Token lp(::mpl::Lexer::Token::TokenType::TT_LEFT_PARENTHESIS, "(");
	::mpl::Lexer::Token rp(::mpl::Lexer::Token::TokenType::TT_RIGHT_PARENTHESIS, ")");
	::mpl::Lexer::Token id(::mpl::Lexer::Token::TokenType::TT_ID, "ID");
	::mpl::Lexer::Token num(::mpl::Lexer::Token::TokenType::TT_NUMBER, "NUM");

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

#endif

#if 0

	::mpl::Lexer::Token T(::mpl::Lexer::Token::TokenType::NONTERMINAL, "T");
	::mpl::Lexer::Token R(::mpl::Lexer::Token::TokenType::NONTERMINAL, "R");
	::mpl::Lexer::Token a(::mpl::Lexer::Token::TokenType::TT_ID, "a");
	::mpl::Lexer::Token b(::mpl::Lexer::Token::TokenType::TT_NUMBER, "b");
	::mpl::Lexer::Token c(::mpl::Lexer::Token::TokenType::TT_STRING, "c");

	rule.clear();
	rule.push_back(R);
	gramma.add(T, rule);

	rule.clear();
	rule.push_back(a);
	rule.push_back(T);
	rule.push_back(c);
	gramma.add(T, rule);

	rule.clear();
	gramma.add(R, rule);

	rule.clear();
	rule.push_back(R);
	rule.push_back(b);
	rule.push_back(R);
	gramma.add(R, rule);

	gramma.build();

#endif
	return 0;
}

#endif