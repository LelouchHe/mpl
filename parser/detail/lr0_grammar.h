#ifndef MPL_PARSER_DETAIL_LR0_GRAMMAR_H
#define MPL_PARSER_DETAIL_LR0_GRAMMAR_H

#include "grammar.h"

#include <utility>
#include <vector>
#include <map>
#include <set>

namespace mpl {
namespace parser {
namespace detail {

class LR0Grammar : public Grammar {
public:
	LR0Grammar();
	~LR0Grammar();

public:
	// <token, ith>
	typedef std::pair<int, int> Rule;
	// <Rule, pos>
	typedef std::pair<Rule, int> Handle;
	typedef std::set<Handle> State;

	// first >= 0: shift, goto second
	// first <  0: reduce, first as token, second as rule
	typedef std::pair<int, int> Action;
	// token, action
	// reduce when token = EPSILON
	typedef std::map<int, Action> Tran;

	bool build();

	const Tran& operator[](size_t state) const;

	void debug() const;

private:
	size_t new_state();

	bool generate_trans();

	void fill(const Handle& handle, State* s);
	size_t expand(const State& from, int token);

private:
	std::vector<State> _states;
	std::vector<Tran> _trans;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LR0_GRAMMAR_H