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

class LR0GrammaOption {
public:
	LR0GrammaOption() : add_fake(false) {

	}

	// true : 增加一个伪头
	// false: 对第一个条规则后加'$'
	bool add_fake;
};

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

	// first >  0: shift, goto second
	// first == 0: accept
	// first <  0: reduce, first as token, second as rule
	typedef std::pair<int, int> Action;
	// token, action
	// reduce when token = EPSILON
	// 因为lr0没有lookahead,所以只能拿EPSILON作为占位字符了
	typedef std::map<int, Action> Tran;

	bool build(LR0GrammaOption option = LR0GrammaOption());

	const Tran& operator[](size_t state) const;

	void debug() const;

	static const int ACCEPT = 0;
	static const int SHIFT = 1;

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