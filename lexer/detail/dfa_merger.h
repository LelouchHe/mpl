#ifndef MPL_LEXER_DETAIL_DFA_MERGER_H
#define MPL_LEXER_DETAIL_DFA_MERGER_H

#include "dfa.h"
#include "dfa_converter.h"

namespace mpl {
namespace lexer {
namespace detail {

class DFAMerger : public DFA {
public:
	DFAMerger();
	~DFAMerger();

	bool add(const DFA& dfa);
	bool build();

public:
	virtual size_t size() const;
	virtual const DFATran& operator[](size_t s) const;
	virtual const Tag& tags(size_t s) const;
	virtual int start() const;
	virtual const StateList& last() const;

private:
	int new_state();

private:
	std::vector<NFATran> _trans;
	std::map<size_t, Tag> _tags;
	int _start;
	StateList _last;

	DFAConverter _converter;
};
	
} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_DFA_MERGER_H