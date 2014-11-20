#ifndef MPL_DETAIL_DFA_H
#define MPL_DETAIL_DFA_H

#include "state.h"

namespace mpl {
namespace detail {

class DFA {
public:
	virtual ~DFA() {}

	virtual size_t size() const = 0;
	virtual const DFATran& operator[](size_t s) const = 0;
	virtual const Tag& tags(size_t s) const = 0;
};

} // namespace detail
} // namespace mpl

#endif // MPL_DETAIL_DFA_H