#include "ll_grammar.h"

#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>

namespace mpl {
namespace parser {
namespace detail {

LLGrammar::LLGrammar() {

}

LLGrammar::~LLGrammar() {

}

void LLGrammar::debug() const {

	Grammar::debug();

	std::cout << " --trans--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _trans.size(); i++) {
		const Tran& tran = _trans[i];
		for (Tran::const_iterator it = tran.begin(); it != tran.end(); ++it) {
			std::cout << _nonterminals[i];
			std::cout << "(" << name(it->first) << ") -> " << it->second << std::endl;
		}
	}
}

const LLGrammar::Tran& LLGrammar::operator[](int token) const {
	token = token2index(token);
	assert(token > 0 && (size_t)token < _nonterminals.size());

	return _trans[token];
}

bool LLGrammar::build(LLGrammaOption option) {

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
	generate_rule_nullable();
	generate_first();
	generate_rule_first();
	generate_follow();

	generate_trans();

	std::cout << " ---- " << std::endl;
	debug();

	std::cout << " ---- " << std::endl;

	return true;
}

// 没有显示的提供EPSILON,而是以empty为标识
// 左递归消除会代码很多额外规则(因为进行了替换),应该可以对那些没有参与递归转换的规则,再转换回去
bool LLGrammar::left_recursion() {
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

bool LLGrammar::left_factor() {
	size_t size = _rules.size();
	for (size_t i = NONTERMINAL_START; i < size; i++) {
		left_factor_token(i);
	}

	return true;
}

bool LLGrammar::left_factor_token(size_t token) {
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

bool LLGrammar::generate_rule_nullable() {
	size_t size = _nonterminals.size();

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

bool LLGrammar::generate_rule_first() {
	size_t size = _nonterminals.size();

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

a ->(c) b

c in FIRST(b)
     FIRST(b) + FOLLOW(a)  nullable(b) = true

#endif

bool LLGrammar::generate_trans() {
	size_t size = _nonterminals.size();
	_trans.resize(size);

	for (size_t left = NONTERMINAL_START; left < size; left++) {
		const InnerRules& rules = _rules[left];

		for (size_t i = 0; i < rules.size(); i++) {
			const InnerRule& rule = rules[i];
			const Tokens& right_first = _rule_first[left][i];

			bool is_nullable = false;
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

const std::vector<LLGrammar::Tran>& LLGrammar::trans() const {
	return _trans;
}

} // namespace detail
} // namespace parser
} // namespace mpl

#if 0

#include <vector>
#include <string>
#include <utility>

using namespace std;

static const vector<pair<string, string> > s_rules = {
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

int main() {
	::mpl::parser::detail::LLGrammar grammar;

	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}
	grammar.build();

	grammar.debug();

	return 0;
}

#endif