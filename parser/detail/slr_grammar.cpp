#include "slr_grammar.h"

#include <iostream>
#include <queue>

namespace mpl {
namespace parser {
namespace detail {

SLRGrammar::SLRGrammar() {

}

SLRGrammar::~SLRGrammar() {

}

void SLRGrammar::debug() const {
	LRGrammar::debug();

	std::cout << " -- state --" << std::endl;
	for (size_t i = 0; i < _states.size(); i++) {
		std::cout << "state[" << i << "]:" << std::endl;

		const State& state = _states[i];
		for (State::const_iterator it = state.begin();
				it != state.end(); ++it) {
			int left = it->first.first;
			const InnerRule& rule = _rules[token2index(left)][it->first.second];
			int pos = it->second;
			const Tokens& tokens = _follow[token2index(left)];

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
size_t SLRGrammar::new_state() {
	size_t size = _states.size();
	_states.resize(size + 1);
	_trans.resize(size + 1);

	return size;
}

bool SLRGrammar::build(SLRGrammarOption options) {
	add_fake_start(!options.add_fake);

	generate_nullable();
	generate_first();
	generate_follow();

	generate_trans();

	return true;
}

bool SLRGrammar::generate_trans() {
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
			size_t rule_no = it->first.second;
			const InnerRule& rule = _rules[token2index(left)][rule_no];
			int pos = it->second;

			if (pos == rule.size()) {
				const Tokens& suffix = _follow[token2index(left)];
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
			size_t end = _states.size();
			size_t next = expand(_states[cur], *it);

			set_tran(*it, SHIFT, next, &_trans[cur]);

			/*
			if ((TokenType)*it == TokenType::EOS) {
				_trans[cur][*it] = { ACCEPT, next };
			} else {
				_trans[cur][*it] = { SHIFT, next };
			}
			*/

			// 感觉有些trick
			if (next >= end) {
				q.push(next);
			}
		}
	}

	return true;
}

void SLRGrammar::fill(const Handle& handle, State* s) {
	s->insert(handle);

	std::queue<Handle> q;
	q.push(handle);

	while (!q.empty()) {
		Handle h = q.front();
		q.pop();

		int left = h.first.first;
		size_t rule_no = h.first.second;
		const InnerRule& rule = _rules[token2index(left)][rule_no];
		size_t pos = h.second;

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
			Handle right_handle = { { right, i }, 0 };

			std::pair<State::iterator, bool> ret = s->insert(right_handle);
			if (ret.second) {
				q.push(right_handle);
			}
		}
	}
}

size_t SLRGrammar::expand(const State& from, int token) {
	State st;

	for (State::const_iterator it = from.begin();
			it != from.end(); ++it) {
		int left = it->first.first;
		size_t rule_no = it->first.second;
		const InnerRule& rule = _rules[token2index(left)][rule_no];
		int pos = it->second;

		if (pos == rule.size()) {
			continue;
		}

		int right = rule[pos];
		if (right != token) {
			continue;
		}

		Handle h = { { left, rule_no }, pos + 1 };
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

int main() {
	::mpl::parser::detail::SLRGrammar grammar;

	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}
	grammar.build();

	grammar.debug();

	return 0;
}

#endif