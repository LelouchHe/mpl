#ifndef MPL_LEXER_DETAIL_DFA_CONVERTER_H
#define MPL_LEXER_DETAIL_DFA_CONVERTER_H

#include "state.h"
#include "dfa.h"

namespace mpl {
namespace lexer {
namespace detail {

// NFA只有一个start,但可能有多个last
class DFAConverter : public DFA {
public:
	DFAConverter();
	~DFAConverter();

	bool parse(const Byte* str);
	bool parse(const Byte* str, int tag);

	bool build(int start, int last,
			const std::vector<NFATran>& trans,
			const std::map<size_t, Tag>& tags);

	bool build(int start, StateList last,
		const std::vector<NFATran>& trans,
		const std::map<size_t, Tag>& tags);
public:
	virtual size_t size() const;
	virtual const DFATran& operator[](size_t s) const;
	virtual const Tag& tags(size_t s) const;
	virtual int start() const;
	virtual const StateList& last() const;

private:
	int new_state();
	void reset();

private:
	std::vector<DFATran> _trans;
	std::map<size_t, Tag> _tags;
	
	int _start;
	StateList _last;
};

} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_DFA_CONVERTER_H