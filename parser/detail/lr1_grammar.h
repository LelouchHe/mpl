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
	typedef std::map<int, size_t> Tran;

	bool build();

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
	std::vector<Tran> _trans;

	// [token][ith rule][pos]
	std::vector<std::vector<std::vector<bool> > > _partial_rule_nullable;
	std::vector<std::vector<std::vector<Tokens> > > _partial_rule_first;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LR1_GRAMMAR_H