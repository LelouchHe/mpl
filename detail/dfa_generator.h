#ifndef MPL_DETAIL_DFA_GENERATOR_H
#define MPL_DETAIL_DFA_GENERATOR_H

#include <set>
#include "dfa.h"

namespace mpl {
namespace detail {

class DFAGenerator : public DFA {
public:
	DFAGenerator();
	~DFAGenerator();

	bool parse(const Byte* str);
	bool parse(const Byte* str, int tag);
	bool build(const DFA& dfa);

public:
	virtual size_t size() const;
	virtual const DFATran& operator[](size_t s) const;
	virtual const Tag& tags(size_t s) const;
	virtual int start() const;
	virtual const StateList& last() const;

private:
	bool build(const DFA& dfa, const std::set<StateList>& t);
	int new_state();
	void reset();

private:
	std::vector<DFATran> _trans;
	std::map<size_t, Tag> _tags;

	int _start;
	StateList _last;
};

} // namespace detail
} // namespace mpl

#endif // MPL_DETAIL_DFA_GENERATOR_H