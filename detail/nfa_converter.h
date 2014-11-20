#ifndef MPL_DETAIL_NFACONVERTER_H
#define MPL_DETAIL_NFACONVERTER_H

#include <vector>
#include <map>
#include <set>

namespace mpl {

class NFAConverter {
public:
	typedef std::vector<int> StateList;
	typedef std::map<char, StateList> NFATran;

	NFAConverter();
	~NFAConverter();

	bool parse(const char* str);

	// Pre: s < size()
	const NFATran& operator[](size_t s) const;
	size_t size() const;

	// < 0: 错误
	int start() const;
	int last() const;

private:
	// return consume byte num
	int build(const char* str, int* start, int* last);
	int build_parenthesis(const char* str, int* start, int* last);
	int build_bracket(const char* str, int* start, int* last);
	int build_escape(const char* str, int* start, int* last);
	int build_dot(const char* str, int* start, int* last);
	int build_char(const char* str, int* start, int* last);
	int build_byte(const char* str, int* start, int* last);

	// 在[]中的处理,以单字符+utf-8
	// 以start/last为始终,接入str
	// []中直接构建DFA(单字符),长串(utf-8)
	int build_escape_direct(const char* str, int* start, int* last);
	int build_char_direct(const char* str, int* start, int* last);
	int build_byte_direct(const char* str, int* start, int* last);

	int build_char_inner(const char* str, int* start, int* last, bool is_direct);

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

	static std::set<char> _s_reserved;
	static bool is_reserved(char ch);

	static std::set<char> _s_suffix;
	static bool is_suffix(char ch);
};

} // namespace mpl

#endif // MPL_DETAIL_NFACONVERTER_H