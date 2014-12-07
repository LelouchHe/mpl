#include "lr1_grammar.h"

#include <iostream>
#include <queue>

namespace mpl {
namespace parser {
namespace detail {

LR1Grammar::LR1Grammar() {

}

LR1Grammar::~LR1Grammar() {

}

void LR1Grammar::debug() const {
	Grammar::debug();

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
size_t LR1Grammar::new_state() {
	size_t size = _states.size();
	_states.resize(size + 1);
	_trans.resize(size + 1);

	return size;
}

bool LR1Grammar::build() {
	//add_fake_start(false);

	generate_nullable();
	generate_partial_rule_nullable();
	generate_first();
	generate_partial_rule_first();

	generate_trans();

	return true;
}

// 0 - size; 最后一位扩充
bool LR1Grammar::generate_partial_rule_nullable() {
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
bool LR1Grammar::generate_partial_rule_first() {
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

bool LR1Grammar::generate_trans() {
	Handle handle = { { { _start, 0 }, 0 }, { TokenType::EOS } };

	size_t cur = new_state();
	fill(handle, &_states[cur]);

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
				continue;
			}

			tokens.insert(rule[pos]);
		}

		for (std::set<int>::const_iterator it = tokens.begin();
				it != tokens.end(); ++it) {
			size_t end = _states.size();
			size_t to = expand(_states[cur], *it);
			_trans[cur][*it] = to;

			// 感觉有些trick
			if (to == end) {
				q.push(to);
			}
		}
	}

	return true;
}

void LR1Grammar::fill(const Handle& handle, State* s) {
	s->insert(handle);

	std::queue<Handle> q;
	q.push(handle);

	while (!q.empty()) {
		Handle h = q.front();
		q.pop();

		int left = h.first.first.first;
		size_t rule_no = h.first.first.second;
		const InnerRule& rule = _rules[token2index(left)][rule_no];
		size_t pos = h.first.second;
		const Tokens& suffix = h.second;

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
			Handle right_handle = { { { right, i }, 0 }, {} };

			right_handle.second.insert(next_first.begin(), next_first.end());

			if (_partial_rule_nullable[token2index(left)][rule_no][pos + 1]) {
				right_handle.second.insert(suffix.begin(), suffix.end());
			}

			std::pair<State::iterator, bool> ret = s->insert(right_handle);
			if (ret.second) {
				q.push(right_handle);
			}
		}
	}
}

size_t LR1Grammar::expand(const State& from, int token) {
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

		Handle h = { { { left, rule_no }, pos + 1 }, suffix };
		fill(h, &st);
	}

	std::vector<State>::iterator it = std::find(_states.begin(), _states.end(), st);
	if (it != _states.end()) {
		return it - _states.begin();
	} else {
		size_t new_s = new_state();
		_states[new_s] = st;
		return new_s;
	}
}

} // namespace detail
} // namespace parser
} // namespace mpl

#if 1

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

	{ "s", "e" },
	{ "e", "t" },
	{ "e", "e '+' t" },
	{ "t", "NUMBER" },
	{ "t", "'(' e ')'" },
};

int main() {
	::mpl::parser::detail::LR1Grammar grammar;

	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}
	grammar.build();

	grammar.debug();

	return 0;
}

#endif