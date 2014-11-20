#ifndef MPL_DFA_CONVERTER_H
#define MPL_DFA_CONVERTER_H

#include "state.h"
#include "dfa.h"

namespace mpl {
namespace detail {

class DFAConverter : public DFA {
public:
	DFAConverter();
	~DFAConverter();

	bool parse(const Byte* str);
	bool parse(const Byte* str, int tag);

	bool build(int start, int last,
			const std::vector<NFATran>& trans,
			const std::map<size_t, Tag>& tags);

	// < 0: ×´Ì¬´íÎó
	int start() const;
	const std::vector<int>& last() const;

public:
	virtual size_t size() const;
	virtual const DFATran& operator[](size_t s) const;
	virtual const Tag& tags(size_t s) const;

private:
	int new_state();
	void reset();

private:
	std::vector<DFATran> _trans;
	std::map<size_t, Tag> _tags;
	
	int _start;
	std::vector<int> _last;
};

} // namespace detail
} // namespace mpl

#endif // MPL_DFA_CONVERTER_H