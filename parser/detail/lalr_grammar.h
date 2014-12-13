#ifndef MPL_PARSER_DETAIL_LALR_GRAMMAR_H
#define MPL_PARSER_DETAIL_LALR_GRAMMAR_H

#include "lr_grammar.h"

#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <set>

namespace mpl {
namespace parser {
namespace detail {

class LALRGrammarOption {
public:
	LALRGrammarOption() : add_fake(false) {

	}

	// true : 增加一个伪头
	// false: 对第一个条规则后加'$'
	bool add_fake;
};

class LALRGrammar : public LRGrammar {
public:
	LALRGrammar();
	~LALRGrammar();

public:
	// <token, ith>
	typedef std::pair<int, int> Rule;
	// <Rule, pos>
	typedef std::pair<Rule, int> Handle;
	typedef std::map<Handle, Tokens> State;

	bool build(LALRGrammarOption options = LALRGrammarOption());

	void debug() const;

private:
	size_t new_state();

	bool generate_partial_rule_nullable();
	bool generate_partial_rule_first();
	bool generate_trans();

	void fill(const Handle& handle, const Tokens& tokens, State* s);
	size_t expand(const State& from, int token, std::queue<size_t>* q);
	void merge(const State& from, size_t to, std::queue<size_t>* q);

private:
	std::vector<State> _states;

	// [token][ith rule][pos]
	std::vector<std::vector<std::vector<bool> > > _partial_rule_nullable;
	std::vector<std::vector<std::vector<Tokens> > > _partial_rule_first;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LALR_GRAMMAR_H