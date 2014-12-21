#ifndef MPL_PARSER_GRAMMAR_LR_GRAMMAR_H
#define MPL_PARSER_GRAMMAR_LR_GRAMMAR_H

#include "grammar.h"

#include <utility>
#include <map>

namespace mpl {
namespace parser {
namespace grammar {

class LRGrammar : public Grammar {
public:
	/*
	LRGrammar();
	~LRGrammar();
	*/

public:
	// first >  0: shift, goto second
	// first == 0: accept
	// first <  0: reduce, first as token, second as rule
	typedef std::pair<int, int> Action;
	// token, action
	// reduce时是lookahead
	// shift时是next
	// 注意区分
	typedef std::map<int, Action> Tran;

	const Tran& operator[](size_t state) const;

	void debug() const;

	const std::vector<Tran> trans() const;

	static const int ACCEPT = 0;
	static const int SHIFT = 1;

protected:
	LRGrammar();
	virtual ~LRGrammar();

	void set_tran(int token, int first, int second, Tran* tran);
	bool has_attr(int token, int first, int second, Attribute* attr);

protected:
	std::vector<Tran> _trans;
};

} // namespace grammar
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_GRAMMAR_LR_GRAMMAR_H