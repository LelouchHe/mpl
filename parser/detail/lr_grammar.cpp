#include "lr_grammar.h"

#include <iostream>
#include <queue>
#include <algorithm>

namespace mpl {
namespace parser {
namespace detail {
	
LRGrammar::LRGrammar() {

}

LRGrammar::~LRGrammar() {

}

bool LRGrammar::build() {
	// add_fake_start();

	Handle handle = { { _start, 0 }, 0 };
	
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
			int left = it->first.first;
			const InnerRule& rule = _rules[token2index(left)][it->first.second];
			int pos = it->second;

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

void LRGrammar::debug() const {
	Grammar::debug();

	for (size_t i = 0; i < _states.size(); i++) {
		std::cout << "state[" << i << "]:" << std::endl;

		const State& state = _states[i];
		for (State::const_iterator it = state.begin();
				it != state.end(); ++it) {
			int left = it->first.first;
			const InnerRule& rule = _rules[token2index(left)][it->first.second];
			int pos = it->second;

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
			std::cout << std::endl;
		}
	}
}

size_t LRGrammar::new_state() {
	size_t size = _states.size();
	_states.resize(size + 1);
	_trans.resize(size + 1);

	return size;
}

// 扩充handle
void LRGrammar::fill(const Handle& handle, State* s) {
	s->insert(handle);

	std::queue<Handle> q;
	q.push(handle);

	while (!q.empty()) {
		Handle h = q.front();
		q.pop();

		int left = h.first.first;
		const InnerRule& rule = _rules[token2index(left)][h.first.second];
		int pos = h.second;

		// 已经终结
		if (pos == rule.size()) {
			continue;
		}

		// terminal
		int right = rule[pos];
		if (right >= 0) {
			continue;
		}

		const InnerRules& right_rules = _rules[token2index(right)];
		for (size_t i = 0; i < right_rules.size(); i++) {
			const InnerRule& right_rule = right_rules[i];
			Handle right_handle = { {right, i}, 0 };

			std::pair<State::iterator, bool> ret = s->insert(right_handle);
			if (ret.second) {
				q.push(right_handle);
			}
		}
	}
}

// 根据返回值判断是否新增了一个状态
size_t LRGrammar::expand(const State& from, int token) {
	State st;

	for (State::const_iterator it = from.begin();
			it != from.end(); ++it) {
		int left = it->first.first;
		const InnerRule& rule = _rules[token2index(left)][it->first.second];
		int pos = it->second;

		if (pos == rule.size()) {
			continue;
		}

		int right = rule[pos];
		if (right != token) {
			continue;
		}

		Handle h = { { left, it->first.second }, pos + 1 };
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
	{ "e", "t ';'" },
	{ "e", "t '+' e" },
	{ "t", "NUMBER" },
	{ "t", "'(' e ')'" },
};

int main() {
	::mpl::parser::detail::LRGrammar grammar;

	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}
	grammar.build();

	grammar.debug();

	return 0;
}

#endif