#ifndef MPL_LEXER_DETAIL_DFA_H
#define MPL_LEXER_DETAIL_DFA_H

#include "state.h"

namespace mpl {
namespace lexer {
namespace detail {

class DFA {
public:
	virtual ~DFA() {}

	virtual size_t size() const = 0;
	virtual const DFATran& operator[](size_t s) const = 0;
	virtual const Tag& tags(size_t s) const = 0;

	virtual int start() const = 0;
	virtual const StateList& last() const = 0;
};

} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_DFA_H