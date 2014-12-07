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
	typedef std::map<int, size_t> Tran;

	bool build();

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