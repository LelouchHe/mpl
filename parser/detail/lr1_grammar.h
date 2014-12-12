#ifndef MPL_PARSER_DETAIL_LR1_GRAMMAR_H
#define MPL_PARSER_DETAIL_LR1_GRAMMAR_H

#include "grammar.h"

#include <utility>
#include <vector>
#include <map>
#include <set>

namespace mpl {
namespace parser {
namespace detail {

class LR1GrammarOption {
public:
	LR1GrammarOption() : add_fake(false) {

	}

	// true : 增加一个伪头
	// false: 对第一个条规则后加'$'
	bool add_fake;
};

class LR1Grammar : public Grammar {
public:
	LR1Grammar();
	~LR1Grammar();

public:
	// <token, ith>
	typedef std::pair<int, int> Rule;
	// <Rule, pos>
	typedef std::pair<Rule, int> Item;
	// <Item, suffix>
	typedef std::pair<Item, Tokens> Handle;
	typedef std::set<Handle> State;

	// first >  0: shift, goto second
	// first == 0: accept
	// first <  0: reduce, first as token, second as rule
	typedef std::pair<int, int> Action;
	// token, action
	// reduce时是lookahead
	// shift时是next
	// 注意区分
	typedef std::map<int, Action> Tran;

	bool build(LR1GrammarOption options = LR1GrammarOption());

	const Tran& operator[](size_t state) const;

	void debug() const;

	static const int ACCEPT = 0;
	static const int SHIFT = 1;

private:
	size_t new_state();

	bool generate_partial_rule_nullable();
	bool generate_partial_rule_first();
	bool generate_trans();

	void fill(const Handle& handle, State* s);
	size_t expand(const State& from, int token);

private:
	std::vector<State> _states;
	std::vector<Tran> _trans;

	// [token][ith rule][pos]
	std::vector<std::vector<std::vector<bool> > > _partial_rule_nullable;
	std::vector<std::vector<std::vector<Tokens> > > _partial_rule_first;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LR1_GRAMMAR_H