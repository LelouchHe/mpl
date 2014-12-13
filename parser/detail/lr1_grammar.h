#ifndef MPL_PARSER_DETAIL_LR1_GRAMMAR_H
#define MPL_PARSER_DETAIL_LR1_GRAMMAR_H

#include "lr_grammar.h"

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

	// true : ����һ��αͷ
	// false: �Ե�һ����������'$'
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
	// lr1��suffixӦ���ǵ�token,�˴���һ��
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

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LR1_GRAMMAR_H