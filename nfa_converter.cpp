#include "nfa_converter.h"
#include <cctype>
#include <cassert>

namespace mpl {

std::set<char> NFAConverter::_s_reserved = {
	'(', ')', '[', ']', '-',
	'*', '+', '?',
	'^', '$', '.'
};

bool NFAConverter::is_reserved(char ch) {
	return _s_reserved.find(ch) != _s_reserved.end();
}

std::set<char> NFAConverter::_s_suffix = {
	'*', '+', '?'
};

bool NFAConverter::is_suffix(char ch) {
	return _s_suffix.find(ch) != _s_suffix.end();
}

NFAConverter::NFAConverter() : _start(-1), _last(-1) {

}

NFAConverter::~NFAConverter() {

}

int NFAConverter::new_state() {
	_trans.resize(_trans.size() + 1);
	return _trans.size() - 1;
}

void NFAConverter::link(int nstart, int nlast, int* start, int* last) {
	if (*start == -1 || *last == -1) {
		*start = nstart;
		*last = nlast;
	} else {
		_trans[*last]['\0'].push_back(nstart);
		*last = nlast;
	}
}

void NFAConverter::link_or(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before]['\0'].push_back(nstart);
	_trans[before]['\0'].push_back(*start);
	_trans[nlast]['\0'].push_back(after);
	_trans[*last]['\0'].push_back(after);

	*start = before;
	*last = after;
}

// start/last是原有状态,将nstart/nlast添加到其间做or
void NFAConverter::link_or_append(int nstart, int nlast, int* start, int* last) {
	_trans[*start]['\0'].push_back(nstart);
	_trans[nlast]['\0'].push_back(*last);
}

void NFAConverter::link_star(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before]['\0'].push_back(nstart);
	_trans[nlast]['\0'].push_back(after);
	_trans[nlast]['\0'].push_back(nstart);
	_trans[before]['\0'].push_back(after);

	*start = before;
	*last = after;
}

void NFAConverter::link_plus(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before]['\0'].push_back(nstart);
	_trans[nlast]['\0'].push_back(after);
	_trans[nlast]['\0'].push_back(nstart);

	*start = before;
	*last = after;
}

void NFAConverter::link_question_mark(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before]['\0'].push_back(nstart);
	_trans[nlast]['\0'].push_back(after);
	_trans[before]['\0'].push_back(after);

	*start = before;
	*last = after;
}

int NFAConverter::build_parenthesis(const char* str, int* start, int* last) {
	assert(*str == '(');

	str++;
	int len = build(str, start, last);
	assert(*(str + len) == ')');
	_catches.push_back(std::make_pair(*start, *last));
	return len + 2;
}

// 目前只处理单字符的选择
// []其实代表着类字符的处理,包括[^ ]
int NFAConverter::build_bracket(const char* str, int* start, int* last) {
	assert(*str == '[');

	const char* begin = str;
	str++;

	*start = new_state();

	if (*str == '^') {
		*last = -1;
		str++;
	} else {
		*last = new_state();
	}

	while (*str != '\0' && *str != ']') {
		int nstart = -1;
		int nlast = -1;
		if (*str == '\\') {
			str += build_escape(str, &nstart, &nlast);
			link_or_append(nstart, nlast, start, last);
		} else {
			// 原始字符
			assert(!is_reserved(*str));

			char from = *str;
			if (*(str + 1) == '-') {
				str += 2;
				assert(!is_reserved(*str));
				assert(from < *str);

				char to[1];
				while (from <= *str) {
					to[0] = from;
					nstart = -1;
					nlast = -1;
					build_char(to, &nstart, &nlast);
					link_or_append(nstart, nlast, start, last);

					from++;
				}
				str++;
			} else {
				str += build_char(str, &nstart, &nlast);
				link_or_append(nstart, nlast, start, last);
			}
		}
	}
	assert(*str == ']');

	// 使用了分类,需要新建\xFF路径
	if (*last == -1) {
		char others[1] = { '\xFF' };
		int nstart = -1;
		int nlast = -1;
		// 注意,此处使用byte,而不是char,避免0xFF被utf-8
		build_byte(others, &nstart, &nlast);
		*last = new_state();
		link_or_append(nstart, nlast, start, last);
	}

	return str + 1 - begin;
}

// 目前只处理reserved的转义
int NFAConverter::build_escape(const char* str, int* start, int* last) {
	assert(*str == '\\');
	assert(is_reserved(*(str + 1)));

	str++;
	int len = build_char(str, start, last);
	return len + 1;
}

int NFAConverter::build_dot(const char* str, int* start, int* last) {
	assert(*str == '.');

	char all[1] = { '\xFF' };
	return build_char(all, start, last);
}

static int left_one(unsigned char ch) {
	int n = 0;
	unsigned int mask = 0x80;
	while (mask > 0 && (ch & mask)) {
		n++;
		mask >>= 1;
	}
	
	return n;
}

int NFAConverter::build_char(const char* str, int* start, int* last) {
	unsigned char uc = *str;
	assert(uc != '\xFF');

	int len = 0;
	if (uc <= '\x7F') {
		len += build_byte(str, start, last);
	} else {
		int n = left_one(uc);
		while (n-- > 0) {
			int nstart = -1;
			int nlast = -1;
			len += build_byte(str, &nstart, &nlast);
			str++;

			link(nstart, nlast, start, last);
		}
	}

	return len;
}

int NFAConverter::build_byte(const char* str, int* start, int* last) {
	*start = new_state();
	*last = new_state();

	_trans[*start][*str].push_back(*last);

	return 1;
}

// 从实现来看,优先级如下:
// 1. 单独节点 (a, (a), \a之类的),包括()
// 2. *节点 (单独处理)
// 3. +节点 (主循环体现)
// 4. |节点 (需要重启build)
int NFAConverter::build(const char* str, int* start, int* last) {
	const char* begin = str;

	while (*str != '\0' && *str != ')') {
		int nstart = -1;
		int nlast = -1;

		// 构建节点(nstart -> nlast)
		// 等效于单一节点,优先级1
		if (*str == '(') {
			str += build_parenthesis(str, &nstart, &nlast);
		} else if (*str == '[') {
			str += build_bracket(str, &nstart, &nlast);
		} else if (*str == '\\') {
			str += build_escape(str, &nstart, &nlast);
		} else if (*str == '.') {
			str += build_dot(str, &nstart, &nlast);
		} else {
			assert(!is_reserved(*str));
			str += build_char(str, &nstart, &nlast);
		}

		// '*'系列单独处理,优先级2
		while (is_suffix(*str)) {
			if (*str == '*') {
				link_star(nstart, nlast, &nstart, &nlast);
			} else if (*str == '+') {
				link_plus(nstart, nlast, &nstart, &nlast);
			} else if (*str == '?') {
				link_question_mark(nstart, nlast, &nstart, &nlast);
			} else {
				assert(false);
			}
			str++;
		}

		// 连接,优先级3
		link(nstart, nlast, start, last);

		// '|',优先级4
		if (*str == '|'){
			str++;
			nstart = -1;
			nlast = -1;
			str += build(str, &nstart, &nlast);
			link_or(nstart, nlast, start, last);
		}
	}

	return str - begin;
}

bool NFAConverter::parse(const char* str) {
	reset();

	int len = build(str, &_start, &_last);
	return str[len] == '\0';
}

void NFAConverter::reset() {
	_trans.clear();
	_start = -1;
	_last = -1;
}

const NFAConverter::NFATran& NFAConverter::operator[](size_t s) const {
	assert(s < _trans.size());
	return _trans[s];
}

size_t NFAConverter::size() const {
	return _trans.size();
}

int NFAConverter::start() const {
	return _start;
}

int NFAConverter::last() const {
	return _last;
}

} // namespace mpl

#if 0

#include <iostream>
using namespace std;

void print_vector(const std::vector<int>& v) {
	cout << "(";
	if (!v.empty()) {
		cout << v[0];
		for (size_t i = 1; i < v.size(); i++) {
			cout << ", " << v[i];
		}
	}
	cout << ")";
}

int main() {
	const char* str = "[_a-zA-Z][_a-zA-Z0-9]*|[ ]";
	::mpl::NFAConverter nfa;
	nfa.parse(str);

	cout << "start: " << nfa.start() << endl;
	cout << "last : " << nfa.last() << endl;

	for (size_t i = 0; i < nfa.size(); i++) {
		const ::mpl::NFAConverter::NFATran& tran = nfa[i];
		for (::mpl::NFAConverter::NFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			cout << i << "(";
			if (it->first == '\0') {
				cout << "\\0";
			} else if (it->first == '\xFF') {
				cout << "-1";
			} else {
				cout << it->first;
				//cout << "0x" << hex << (int)(it->first & 0xFF) << dec;
			}
			cout << ")";
			cout << "\t->\t";
			print_vector(it->second);
			cout << endl;
		}
	}

	return 0;
}

#endif