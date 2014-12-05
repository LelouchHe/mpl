#ifndef MPL_PARSER_DETAIL_LR_GRAMMAR_H
#define MPL_PARSER_DETAIL_LR_GRAMMAR_H

#include "grammar.h"

#include <utility>
#include <vector>
#include <map>
#include <set>

namespace mpl {
namespace parser {
namespace detail {

class LRGrammar : public Grammar {
public:
	LRGrammar();
	~LRGrammar();

public:
	// <token, ith rule>
	typedef std::pair<int, int> Item;
	typedef set<Item> State;

	bool build();

	void debug() const;

private:
	size_t new_state();

private:
	std::vector<State> _states;
	std::vector<std::map<int, int> > _trans;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LR_GRAMMAR_H