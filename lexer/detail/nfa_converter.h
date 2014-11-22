#ifndef MPL_LEXER_DETAIL_NFACONVERTER_H
#define MPL_LEXER_DETAIL_NFACONVERTER_H

#include <set>
#include "state.h"

namespace mpl {
namespace lexer {
namespace detail {

class NFAConverter {
public:
	NFAConverter();
	~NFAConverter();

	bool parse(const Byte* str);

	const std::vector<NFATran>& trans() const;

	size_t size() const;
	const NFATran& operator[](size_t s) const;

	// < 0: 错误
	int start() const;
	int last() const;

private:
	// return consume byte num
	int build(const Byte* str, int* start, int* last);
	int build_parenthesis(const Byte* str, int* start, int* last);
	int build_bracket(const Byte* str, int* start, int* last);
	int build_escape(const Byte* str, int* start, int* last);
	int build_dot(const Byte* str, int* start, int* last);
	int build_char(const Byte* str, int* start, int* last);
	int build_byte(const Byte* str, int* start, int* last);

	// 在[]中的处理,以单字符+utf-8
	// 以start/last为始终,接入str
	// []中直接构建DFA(单字符),长串(utf-8)
	int build_escape_direct(const Byte* str, int* start, int* last);
	int build_char_direct(const Byte* str, int* start, int* last);
	int build_byte_direct(const Byte* str, int* start, int* last);

	int build_char_inner(const Byte* str, int* start, int* last, bool is_direct);

	void link(int nstart, int nlast, int* start, int* last);
	void link_star(int nstart, int nlast, int* start, int* last);
	void link_plus(int nstart, int nlast, int* start, int* last);
	void link_question_mark(int nstart, int nlast, int* start, int* last);
	void link_or(int nstart, int nlast, int* start, int* last);
	void link_or_append(int nstart, int nlast, int* start, int* last);

	int new_state();
	void reset();

private:
	std::vector<NFATran> _trans;
	int _start;
	int _last;
};

} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_NFACONVERTER_H