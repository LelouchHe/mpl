#include "nfa_converter.h"
#include <cctype>
#include <cassert>

namespace mpl {
namespace lexer {
namespace detail {

static std::set<Byte> s_reserved = {
	'(', ')', '[', ']', '-',
	'*', '+', '?',
	'^', '$', '.'
};

static bool is_reserved(Byte ch) {
	return s_reserved.find(ch) != s_reserved.end();
}

static std::set<Byte> s_suffix = {
	'*', '+', '?'
};

static bool is_suffix(Byte ch) {
	return s_suffix.find(ch) != s_suffix.end();
}

// ��εõ�ԭ�����޷����ַ���?
static std::map<Byte, const char *> s_char_class = {
	{ 'w', "[_a-zA-Z0-9]" }, { 'a', "[a-zA-Z]" },
	{ 'd', "[0-9]" }, { 'x', "[0-9a-fA-F]" },
	{ 's', "[ \t]" }, { 'n', "\n|\n\r|\r\n" },
	{ 'W', "[^\\w]" }, { 'A', "[^\\a]" },
	{ 'D', "[^\\d]" }, { 'X', "[^\\x]" },
	{ 'S', "[^\\s]" }, { 'N', "[^\\n]" }
};

NFAConverter::NFAConverter() : _start(-1), _last(-1) {

}

NFAConverter::~NFAConverter() {

}

int NFAConverter::new_state() {
	_trans.resize(_trans.size() + 1);
	return _trans.size() - 1;
}

void NFAConverter::link(int nstart, int nlast, int* start, int* last) {
	// last�п�����-1,��ʾ����״̬
	if (*start == -1) {
		*start = nstart;
		*last = nlast;
	} else {
		_trans[*last][EPSILON].insert(nstart);
		*last = nlast;
	}
}

void NFAConverter::link_or(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before][EPSILON].insert(nstart);
	_trans[before][EPSILON].insert(*start);
	_trans[nlast][EPSILON].insert(after);
	_trans[*last][EPSILON].insert(after);

	*start = before;
	*last = after;
}

// start/last��ԭ��״̬,��nstart/nlast���ӵ������or
void NFAConverter::link_or_append(int nstart, int nlast, int* start, int* last) {
	_trans[*start][EPSILON].insert(nstart);
	_trans[nlast][EPSILON].insert(*last);
}

void NFAConverter::link_star(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before][EPSILON].insert(nstart);
	_trans[nlast][EPSILON].insert(after);
	_trans[nlast][EPSILON].insert(nstart);
	_trans[before][EPSILON].insert(after);

	*start = before;
	*last = after;
}

void NFAConverter::link_plus(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before][EPSILON].insert(nstart);
	_trans[nlast][EPSILON].insert(after);
	_trans[nlast][EPSILON].insert(nstart);

	*start = before;
	*last = after;
}

void NFAConverter::link_question_mark(int nstart, int nlast, int* start, int* last) {
	int before = new_state();
	int after = new_state();

	_trans[before][EPSILON].insert(nstart);
	_trans[nlast][EPSILON].insert(after);
	_trans[before][EPSILON].insert(after);

	*start = before;
	*last = after;
}

int NFAConverter::build_parenthesis(const Byte* str, int* start, int* last) {
	assert(*str == '(');

	str++;
	int len = build(str, start, last);
	assert(*(str + len) == ')');
	return len + 2;
}

// Ŀǰֻ�������ַ���ѡ��
// []��ʵ���������ַ��Ĵ���,����[^ ]
// direct��Ŀ�����ڼ���״̬,ʹ����ʼ����һ��״̬,������������״̬����,�ȽϿ���(����,����������ǰ�����ܿ�)
// ���뵽2�ֽ��:
// 1. ������������,����ʱ�;����ϲ�״̬
// 2. �ճ�����,���ϲ���ͬ����ʼ״̬,�����Ϊdead state,�����������к���
// �������õ�re,������ʵ�ȼ�
// ����,��Ҫд[\d0-9]�����д����ظ���ת��
// �ð�,ѡ��������ͼʹ�������,��reӦ�ú���Ϥ
int NFAConverter::build_bracket(const Byte* str, int* start, int* last) {
	assert(*str == '[');

	const Byte* begin = str;
	str++;

	*start = new_state();

	if (*str == '^') {
		*last = -1;
		str++;
	} else {
		*last = new_state();
	}

	while (*str != '\0' && *str != ']') {
		if (*str == '\\') {
			str += build_escape_direct(str, start, last);
		} else {
			// ԭʼ�ַ�
			assert(!is_reserved(*str));

			Byte from = *str;
			if (*(str + 1) == '-') {
				str += 2;
				assert(!is_reserved(*str));
				assert(from < *str);

				Byte to[1];
				while (from <= *str) {
					to[0] = from;
					build_byte_direct(to, start, last);

					from++;
				}
				str++;
			} else {
				str += build_byte_direct(str, start, last);
			}
		}
	}
	assert(*str == ']');

	// ʹ���˷���,��Ҫ�½�\xFF·��
	if (*last == -1) {
		Byte others[1] = { OTHER };
		// ע��,�˴�ʹ��byte,������Byte,����0xFF��utf-8
		*last = new_state();
		build_byte_direct(others, start, last);
	}

	return str + 1 - begin;
}

// Ŀǰֻ����reserved/char_class��ת��
int NFAConverter::build_escape(const Byte* str, int* start, int* last) {
	assert(*str == '\\');

	str++;
	int len = 0;
	if (is_reserved(*str)) {
		len = build_byte(str, start, last);
	} else {
		std::map<Byte, const char *>::const_iterator it = s_char_class.find(*str);
		assert(it != s_char_class.end());
		build((const Byte *)it->second, start, last);
		len = 1;
	}

	return len + 1;
}

// Ŀǰֻ����reserved��ת��
int NFAConverter::build_escape_direct(const Byte* str, int* start, int* last) {
	assert(*str == '\\');

	str++;
	int len = 0;
	if (is_reserved(*str)) {
		len = build_byte_direct(str, start, last);
	} else {
		std::map<Byte, const char *>::const_iterator it = s_char_class.find(*str);
		assert(it != s_char_class.end());
		int nstart = -1;
		int nlast = -1;
		build((const Byte *)it->second, &nstart, &nlast);
		link_or_append(nstart, nlast, start, last);
		len = 1;
	}

	return len + 1;
}

int NFAConverter::build_dot(const Byte* str, int* start, int* last) {
	assert(*str == '.');

	Byte all[1] = { OTHER };
	return build_byte(all, start, last);
}

static int left_one(Byte ch) {
	int n = 0;
	unsigned int mask = 0x80;
	while (mask > 0 && (ch & mask)) {
		n++;
		mask >>= 1;
	}
	
	return n;
}

int NFAConverter::build_char(const Byte* str, int* start, int* last) {
	return build_char_inner(str, start, last, false);
}

int NFAConverter::build_char_direct(const Byte* str, int* start, int* last) {
	return build_char_inner(str, start, last, true);
}

int NFAConverter::build_char_inner(const Byte* str, int* start, int* last, bool is_direct) {
	Byte uc = *str;
	assert(uc != OTHER);

	int len = 0;
	if (uc <= 0x7F) {
		if (is_direct) {
			len += build_byte_direct(str, start, last);
		} else {
			len += build_byte(str, start, last);
		}
	} else {
		int n = left_one(uc);

		// vs ���� utf-8 ������,����ʱʹ�� ucs ˫�ֽڱ�ʾ
		// n = 2;
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

int NFAConverter::build_byte(const Byte* str, int* start, int* last) {
	*start = new_state();
	*last = new_state();

	return build_byte_direct(str, start, last);
}

int NFAConverter::build_byte_direct(const Byte* str, int* start, int* last) {
	_trans[*start][*str].insert(*last);
	return 1;
}

// ��ʵ������,���ȼ�����:
// 1. �����ڵ� (a, (a), \a֮���),����()
// 2. *�ڵ� (��������)
// 3. +�ڵ� (��ѭ������)
// 4. |�ڵ� (��Ҫ����build)
int NFAConverter::build(const Byte* str, int* start, int* last) {
	const Byte* begin = str;

	while (*str != '\0' && *str != ')') {
		int nstart = -1;
		int nlast = -1;

		// �����ڵ�(nstart -> nlast)
		// ��Ч�ڵ�һ�ڵ�,���ȼ�1
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

		// '*'ϵ�е�������,���ȼ�2
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

		// ����,���ȼ�3
		link(nstart, nlast, start, last);

		// '|',���ȼ�4
		if (*str == '|'){
			str++;
			nstart = -1;
			nlast = -1;
			str += build(str, &nstart, &nlast);
			link_or_append(nstart, nlast, start, last);
		}
	}

	return str - begin;
}

bool NFAConverter::parse(const Byte* str) {
	reset();

	int len = build(str, &_start, &_last);
	return str[len] == '\0';
}

void NFAConverter::reset() {
	_trans.clear();
	_start = -1;
	_last = -1;
}

const std::vector<NFATran>& NFAConverter::trans() const {
	return _trans;
}

const NFATran& NFAConverter::operator[](size_t s) const {
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

} // namespace detail
} // namespace lexer
} // namespace mpl

#if 0

#include <iostream>
using namespace std;

void print_set(const std::set<int>& s) {
	cout << "(";
	if (!s.empty()) {
		size_t size = s.size();
		size_t i = 0;
		for (::mpl::lexer::detail::States::const_iterator it = s.begin();
			it != s.end(); ++it) {
			if (i > 0) {
				cout << ", ";
			}
			cout << *it;
			i++;
		}
	}
	cout << ")";
}

int main() {
	const char* str = "\\N";
	::mpl::lexer::detail::NFAConverter nfa;
	nfa.parse((const ::mpl::lexer::detail::Byte*)str);

	cout << "start: " << nfa.start() << endl;
	cout << "last : " << nfa.last() << endl;

	for (size_t i = 0; i < nfa.size(); i++) {
		const ::mpl::lexer::detail::NFATran& tran = nfa[i];
		for (::mpl::lexer::detail::NFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			cout << i << "(";
			if (it->first == ::mpl::lexer::detail::EPSILON) {
				cout << "\\0";
			} else if (it->first == ::mpl::lexer::detail::OTHER) {
				cout << "-1";
			} else {
				//cout << it->first;
				cout << "0x" << hex << (int)(it->first & 0xFF) << dec;
			}
			cout << ")";
			cout << "\t->\t";
			print_set(it->second);
			cout << endl;
		}
	}

	return 0;
}

#endif