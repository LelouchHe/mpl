#ifndef MPL_PARSER_GRAMMAR_LR1_GRAMMAR_H
#define MPL_PARSER_GRAMMAR_LR1_GRAMMAR_H

#include "lr_grammar.h"

#include <utility>
#include <vector>
#include <map>
#include <set>

namespace mpl {
namespace parser {
namespace grammar {

class LR1GrammarOption {
public:
	LR1GrammarOption() : add_fake(false) {

	}

	// true : 增加一个伪头
	// false: 对第一个条规则后加'$'
	bool add_fake;
};

class LR1Grammar : public LRGrammar {
public:
	LR1Grammar();
	~LR1Grammar();

public:
	// <token, ith>
	typedef std::pair<int, int> Rule;
	// <Rule, pos>
	typedef std::pair<Rule, int> Item;
	// <Item, suffix>
	// lr1的suffix应该是单token,此处简化一下
	typedef std::pair<Item, Tokens> Handle;
	typedef std::set<Handle> State;

	bool build(LR1GrammarOption options = LR1GrammarOption());

	void debug() const;

private:
	size_t new_state();

	bool generate_partial_rule_nullable();
	bool generate_partial_rule_first();
	bool generate_trans();

	void fill(const Handle& handle, State* s);
	size_t expand(const State& from, int token);

private:
	std::vector<State> _states;

	// [token][ith rule][pos]
	std::vector<std::vector<std::vector<bool> > > _partial_rule_nullable;
	std::vector<std::vector<std::vector<Tokens> > > _partial_rule_first;
};

} // namespace grammar
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_GRAMMAR_LR1_GRAMMAR_H