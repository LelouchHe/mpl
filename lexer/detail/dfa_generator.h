#ifndef MPL_LEXER_DETAIL_DFA_GENERATOR_H
#define MPL_LEXER_DETAIL_DFA_GENERATOR_H

#include <set>
#include "dfa.h"

namespace mpl {
namespace lexer {
namespace detail {

class DFAGenerator : public DFA {
public:
	DFAGenerator();
	~DFAGenerator();

	bool parse(const Byte* str);
	bool parse(const Byte* str, int tag);
	bool build(const DFA& dfa, bool is_sep);

public:
	virtual size_t size() const;
	virtual const DFATran& operator[](size_t s) const;
	virtual const Tag& tags(size_t s) const;
	virtual int start() const;
	virtual const States& last() const;

private:
	bool build(const DFA& dfa, const std::set<States>& t);
	int new_state();
	void reset();

private:
	std::vector<DFATran> _trans;
	std::map<size_t, Tag> _tags;

	int _start;
	States _last;
};

} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_DFA_GENERATOR_H