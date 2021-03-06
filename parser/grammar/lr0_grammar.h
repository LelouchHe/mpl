#ifndef MPL_PARSER_GRAMMAR_LR0_GRAMMAR_H
#define MPL_PARSER_GRAMMAR_LR0_GRAMMAR_H

#include "lr_grammar.h"

#include <utility>
#include <vector>
#include <map>
#include <set>

namespace mpl {
namespace parser {
namespace grammar {

class LR0GrammarOption {
public:
	LR0GrammarOption() : add_fake(false) {

	}

	// true : 增加一个伪头
	// false: 对第一个条规则后加'$'
	bool add_fake;
};

class LR0Grammar : public LRGrammar {
public:
	LR0Grammar();
	~LR0Grammar();

public:
	// <token, ith>
	typedef std::pair<int, int> Rule;
	// <Rule, pos>
	typedef std::pair<Rule, int> Handle;
	typedef std::set<Handle> State;

	bool build(LR0GrammarOption option = LR0GrammarOption());

	void debug() const;

private:
	size_t new_state();

	bool generate_trans();

	void fill(const Handle& handle, State* s);
	size_t expand(const State& from, int token);

private:
	std::vector<State> _states;
};

} // namespace grammar
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_GRAMMAR_LR0_GRAMMAR_H