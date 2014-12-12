#ifndef MPL_PARSER_DETAIL_LALR_GRAMMAR_H
#define MPL_PARSER_DETAIL_LALR_GRAMMAR_H

#include "grammar.h"

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

class LALRGrammar : public Grammar {
public:
	LALRGrammar();
	~LALRGrammar();

public:
	// <token, ith>
	typedef std::pair<int, int> Rule;
	// <Rule, pos>
	typedef std::pair<Rule, int> Handle;
	typedef std::map<Handle, Tokens> State;

	// first >  0: shift, goto second
	// first == 0: accept
	// first <  0: reduce, first as token, second as rule
	typedef std::pair<int, int> Action;
	// token, action
	// reduce时是lookahead
	// shift时是next
	// 注意区分
	typedef std::map<int, Action> Tran;

	bool build(LALRGrammarOption options = LALRGrammarOption());

	const Tran& operator[](size_t state) const;

	void debug() const;

	static const int ACCEPT = 0;
	static const int SHIFT = 1;

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
	std::vector<Tran> _trans;

	// [token][ith rule][pos]
	std::vector<std::vector<std::vector<bool> > > _partial_rule_nullable;
	std::vector<std::vector<std::vector<Tokens> > > _partial_rule_first;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LALR_GRAMMAR_H