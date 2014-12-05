#include "lr_grammar.h"

namespace mpl {
namespace parser {
namespace detail {
	
LRGrammar::LRGrammar() {

}

LRGrammar::~LRGrammar() {

}

bool LRGrammar::build() {
	add_fake_start();

	return true;
}

void LRGrammar::debug() const {
	Grammar::debug();
}

size_t LRGrammar::new_state() {
	size_t size = _states.size();
	_states.resize(size + 1);
	_trans.resize(size + 1);

	return size;
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
	::mpl::parser::detail::LRGrammar grammar;

	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}
	grammar.build();

	grammar.debug();

	return 0;
}

#endif