#include "lalr_grammar.h"

#include <iostream>
#include <queue>

namespace mpl {
namespace parser {
namespace grammar {

LALRGrammar::LALRGrammar() {

}

LALRGrammar::~LALRGrammar() {

}

void LALRGrammar::debug() const {
	LRGrammar::debug();
	
	std::cout << " -- state --" << std::endl;
	for (size_t i = 0; i < _states.size(); i++) {
		std::cout << "state[" << i << "]:" << std::endl;

		const State& state = _states[i];
		for (State::const_iterator it = state.begin();
				it != state.end(); ++it) {
			int left = it->first.first.first;
			const InnerRule& rule = _rules[token2index(left)][it->first.first.second];
			int pos = it->first.second;
			const Tokens& tokens = it->second;

			std::cout << name(left) << " : ";
			for (size_t j = 0; j < rule.size(); j++) {
				if (j == pos) {
					std::cout << "@";
				}
				std::cout << name(rule[j]) << " ";
			}
			if (pos == rule.size()) {
				std::cout << "@";
			}
			std::cout << "\t(";
			for (Tokens::const_iterator tit = tokens.begin();
					tit != tokens.end(); ++tit) {
				std::cout << name(*tit) << ", ";
			}
			std::cout << ")";
			std::cout << std::endl;
		}
	}
}
size_t LALRGrammar::new_state() {
	size_t size = _states.size();
	_states.resize(size + 1);
	_trans.resize(size + 1);

	return size;
}

bool LALRGrammar::build(LALRGrammarOption option) {
	add_fake_start(!option.add_fake);

	generate_nullable();
	generate_partial_rule_nullable();
	generate_first();
	generate_partial_rule_first();

	generate_trans();

	return true;
}

// 0 - size; 最后一位扩充
bool LALRGrammar::generate_partial_rule_nullable() {
	size_t size = _nonterminals.size();

	_partial_rule_nullable.resize(size);
	for (size_t left = NONTERMINAL_START; left < size; left++) {
		const InnerRules& rules = _rules[left];
		_partial_rule_nullable[left].resize(rules.size());

		for (size_t i = 0; i < rules.size(); i++) {
			const InnerRule& rule = rules[i];
			_partial_rule_nullable[left][i].resize(rule.size() + 1);

			bool is_nullable = true;
			for (int pos = rule.size() - 1; pos >= 0; pos--) {
				if (rule[pos] >= 0) {
					_partial_rule_nullable[left][i][pos] = false;
					is_nullable = false;
				} else if (is_nullable && _nullable[token2index(rule[pos])]) {
					_partial_rule_nullable[left][i][pos] = true;
				} else {
					_partial_rule_nullable[left][i][pos] = false;
					is_nullable = false;
				}
			}
			_partial_rule_nullable[left][i][rule.size()] = true;
		}

	}

	return true;
}

// 0 - size; 最后一位扩充
bool LALRGrammar::generate_partial_rule_first() {
	size_t size = _nonterminals.size();

	_partial_rule_first.resize(size);
	for (size_t left = NONTERMINAL_START; left < size; left++) {
		const InnerRules& rules = _rules[left];
		_partial_rule_first[left].resize(rules.size());

		for (size_t i = 0; i < rules.size(); i++) {
			const InnerRule& rule = rules[i];
			_partial_rule_first[left][i].resize(rule.size() + 1);

			std::vector<Tokens>& first_tokens = _partial_rule_first[left][i];

			for (int pos = rule.size() - 1; pos >= 0; pos--) {
				if (rule[pos] >= 0) {
					first_tokens[pos].insert((TokenType)rule[pos]);
				} else {
					first_tokens[pos].insert(_first[token2index(rule[pos])].begin(),
								_first[token2index(rule[pos])].end());
					if ((size_t)pos < rule.size() - 1) {
						first_tokens[pos].insert(first_tokens[pos + 1].begin(),
								first_tokens[pos + 1].end());
					}
				}
			}
		}
	}

	return true;
}

bool LALRGrammar::generate_trans() {
	Handle start_handle = { { _start, 0 }, 0 };
	Tokens start_suffix = { TokenType::EOS };

	size_t cur = new_state();
	fill(start_handle, start_suffix, &_states[cur]);

	std::queue<size_t> q;
	q.push(cur);
	while (!q.empty()) {
		cur = q.front();
		q.pop();

		std::set<int> tokens;
		for (State::const_iterator it = _states[cur].begin();
				it != _states[cur].end(); ++it) {
			int left = it->first.first.first;
			size_t rule_no = it->first.first.second;
			const InnerRule& rule = _rules[token2index(left)][rule_no];
			int pos = it->first.second;
			const Tokens& suffix = it->second;

			if (pos == rule.size()) {
				for (Tokens::const_iterator tit = suffix.begin();
						tit != suffix.end(); ++tit) {
					//_trans[cur][*tit] = { left, rule_no };
					set_tran(*tit, left, rule_no, &_trans[cur]);
				}
				continue;
			}

			tokens.insert(rule[pos]);
		}

		for (std::set<int>::const_iterator it = tokens.begin();
				it != tokens.end(); ++it) {
			size_t next = expand(_states[cur], *it, &q);

			set_tran(*it, SHIFT, next, &_trans[cur]);
			
			/*
			if ((TokenType)*it == TokenType::EOS) {
				_trans[cur][*it] = { ACCEPT, next };
			} else {
				_trans[cur][*it] = { SHIFT, next };
			}
			*/
		}
	}

	return true;
}

void LALRGrammar::fill(const Handle& handle, const Tokens& tokens, State* s) {
	(*s)[handle].insert(tokens.begin(), tokens.end());

	std::queue<Handle> q;
	q.push(handle);

	while (!q.empty()) {
		Handle h = q.front();
		q.pop();

		int left = h.first.first;
		size_t rule_no = h.first.second;
		const InnerRule& rule = _rules[token2index(left)][rule_no];
		size_t pos = h.second;
		const Tokens& suffix = (*s)[h];

		// 已经终结
		if (pos == rule.size()) {
			continue;
		}

		// terminal
		int right = rule[pos];
		if (right >= 0) {
			continue;
		}

		const Tokens& next_first = _partial_rule_first[token2index(left)][rule_no][pos + 1];

		const InnerRules& right_rules = _rules[token2index(right)];
		for (size_t i = 0; i < right_rules.size(); i++) {
			const InnerRule& right_rule = right_rules[i];
			Handle right_handle = { { right, i }, 0 };
			Tokens right_suffix(next_first);

			if (_partial_rule_nullable[token2index(left)][rule_no][pos + 1]) {
				right_suffix.insert(suffix.begin(), suffix.end());
			}

			std::map<Handle, Tokens>::iterator ret = s->find(right_handle);
			if (ret == s->end()) {
				(*s)[right_handle] = right_suffix;
				q.push(right_handle);
			} else {
				ret->second.insert(right_suffix.begin(), right_suffix.end());
			}
		}
	}
}

// 没找到的话,返回states.size()
static size_t find(const std::vector<LALRGrammar::State>& states, const LALRGrammar::State& st) {
	size_t i = 0;
	for (i = 0; i < states.size(); i++) {
		const LALRGrammar::State& t = states[i];
		if (t.size() != st.size()) {
			continue;
		}

		LALRGrammar::State::const_iterator t_it = t.begin();
		LALRGrammar::State::const_iterator st_it = st.begin();
		while (t_it != t.end() && st_it != st.end()) {
			if (t_it->first != st_it->first) {
				break;
			}
			t_it++;
			st_it++;
		}

		if (t_it == t.end() && st_it == st.end()) {
			break;
		}
	}

	return i;
}

size_t LALRGrammar::expand(const State& from, int token, std::queue<size_t>* q) {
	State st;

	for (State::const_iterator it = from.begin();
			it != from.end(); ++it) {
		int left = it->first.first.first;
		size_t rule_no = it->first.first.second;
		const InnerRule& rule = _rules[token2index(left)][rule_no];
		int pos = it->first.second;
		const Tokens& suffix = it->second;

		if (pos == rule.size()) {
			continue;
		}

		int right = rule[pos];
		if (right != token) {
			continue;
		}

		Handle h = { { left, rule_no }, pos + 1 };
		fill(h, suffix, &st);
	}

	size_t new_s = find(_states, st);
	if (new_s < _states.size()) {
		merge(st, new_s, q);
		return new_s;
	} else {
		size_t new_s = new_state();
		_states[new_s] = st;
		q->push(new_s);
		return new_s;
	}
}

// 1. 状态的改变需要重复更新
// 2. trans只针对reduce更新(shift的状态改变是固定唯一的)
void LALRGrammar::merge(const State& from, size_t to, std::queue<size_t>* q) {
	bool is_updated = false;
	for (State::const_iterator it = from.begin();
			it != from.end(); ++it) {
		int left = it->first.first.first;
		size_t rule_no = it->first.first.second;
		const InnerRule& rule = _rules[token2index(left)][rule_no];
		int pos = it->first.second;
		const Tokens& suffix = it->second;

		std::pair<Tokens::iterator, bool> ret;
		for (Tokens::const_iterator tit = suffix.begin();
				tit != suffix.end(); ++tit) {
			if (pos == rule.size()) {
				//_trans[to][*tit] = { left, rule_no };
				set_tran(*tit, left, rule_no, &_trans[to]);
			}

			// 需要监控state有无更新
			ret = _states[to][it->first].insert(*tit);
			if (ret.second) {
				is_updated = true;
			}
		}
	}

	if (is_updated) {
		q->push(to);
	}
}

} // namespace grammar
} // namespace parser
} // namespace mpl

#if 0

#include <vector>
#include <string>
#include <utility>

using namespace std;

static const vector<pair<string, string> > s_rules = {
	/*
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
	*/

	/*
	{ "s", "e" },
	{ "e", "t" },
	{ "e", "e '+' t" },
	{ "t", "NUMBER" },
	{ "t", "'(' e ')'" },
	*/

	/*
	{ "s", "e" },
	{ "e", "l '=' r" },
	{ "e", "r" },
	{ "l", "ID" },
	{ "l", "'*' r" },
	{ "r", "l" },
	*/

	// LR1
	// has reduce/reduce conflict
	/*
	{ "S", "NUMBER E NUMBER" },
	{ "S", "STRING E STRING" },
	{ "S", "NUMBER F STRING" },
	{ "S", "STRING F NUMBER" },
	{ "E", "ID" },
	{ "F", "ID" },
	*/

	{ "s", "e" },
	{ "e", "e '+' e" },
	{ "e", "e '*' e" },
	{ "e", "NUMBER" },
};

int main() {
	::mpl::parser::grammar::LALRGrammar grammar;

	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}

	grammar.add("'+'", 1, ::mpl::parser::grammar::LALRGrammar::Associativity::LEFT);
	grammar.add("'*'", 2, ::mpl::parser::grammar::LALRGrammar::Associativity::LEFT);

	::mpl::parser::grammar::LALRGrammarOption option;
	option.add_fake = false;
	grammar.build(option);

	grammar.debug();

	return 0;
}

#endif