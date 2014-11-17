#ifndef MPL_NFAConverter_H
#define MPL_NFAConverter_H

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

	// < 0: 状态错误
	int start() const;
	int last() const;

private:
	// 返回的是消耗掉的字符数
	int build(const char* str, int* start, int* last);
	int build_parenthesis(const char* str, int* start, int* last);
	int build_bracket(const char* str, int* start, int* last);
	int build_escape(const char* str, int* start, int* last);
	int build_alpha(const char* str, int* start, int* last);

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

	typedef std::pair<int, int> Catch;
	std::vector<Catch> _catches;

	static std::set<char> _s_reserved;
	static bool is_reserved(char ch);

	static std::set<char> _s_suffix;
	static bool is_suffix(char ch);
};

} // namespace mpl

#endif // MPL_NFAConverter_H