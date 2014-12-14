#include "lr_grammar.h"

#include <iostream>

namespace mpl {
namespace parser {
namespace grammar {

LRGrammar::LRGrammar() {

}

LRGrammar::~LRGrammar() {

}

void LRGrammar::debug() const {
	Grammar::debug();

	std::cout << " -- tran --" << std::endl;
	for (size_t i = 0; i < _trans.size(); i++) {
		std::cout << "state[" << i << "]:" << std::endl;

		const Tran& tran = _trans[i];
		for (Tran::const_iterator it = tran.begin();
			it != tran.end(); ++it) {
			std::cout << "(" << name(it->first) << "): ";
			const Action& action = it->second;
			if (action.first == ACCEPT) {
				std::cout << "accept";
			} else if (action.first == SHIFT) {
				std::cout << "shift -> " << action.second;
			} else {
				std::cout << "reduce -> (" << name(action.first) << ", " << action.second << ")";
			}

			std::cout << std::endl;
		}
	}
}

const LRGrammar::Tran& LRGrammar::operator[](size_t state) const {
	assert(state < _trans.size());

	return _trans[state];
}

bool LRGrammar::has_attr(int token, int first, int second, Attribute* attr) {
	if (first == SHIFT) {
		if (_attrs.find(token) == _attrs.end()) {
			return false;
		}
		*attr = _attrs[token];
	} else if (first < 0) {
		const InnerRule& rule = _rules[token2index(first)][second];
		// 目前只针对binary操作符
		if (rule.size() != 3 || _attrs.find(rule[1]) == _attrs.end()) {
			return false;
		}
		*attr = _attrs[rule[1]];
	}

	return true;
}

void LRGrammar::set_tran(int token, int first, int second, Tran* tran) {
	if (first == SHIFT && (TokenType)token == TokenType::EOS) {
		(*tran)[token] = { ACCEPT, second };
		return;
	}

	Tran::iterator it = tran->find(token);
	if (it == tran->end()) {
		(*tran)[token] = { first, second };
		return;
	}

	if (first == it->second.first && second == it->second.second) {
		return;
	}

	// 必须是shift/reduce冲突
	assert(first * it->second.first < 0);

	Attribute token_attr;
	if (!has_attr(token, first, second, &token_attr)) {
		return;
	}

	Action current_action = it->second;
	Attribute current_attr;
	if (!has_attr(token, current_action.first, current_action.second, &current_attr)) {
		(*tran)[token] = { first, second };
		return;
	}

	Action real_action = { first, second };
	if (token_attr.first <= current_attr.first) {
		if (token_attr.first < current_attr.first) {
			real_action = current_action;
		} else if (token_attr.second == Associativity::LEFT) {
			if (first == SHIFT) {
				real_action = current_action;
			}
		} else if (token_attr.second == Associativity::RIGHT) {
			if (first < 0) {
				real_action = current_action;
			}
		}
	}

	(*tran)[token] = real_action;
}

} // namespace grammar
} // namespace parser
} // namespace mpl