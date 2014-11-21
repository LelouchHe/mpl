#include "regex.h"
#include <algorithm>

namespace mpl {
namespace detail {

Regex::Regex() {

}

Regex::~Regex() {

}

bool Regex::parse(const char* pattern) {
	_pattern.assign(pattern);
	
	return _dfa.parse((const Byte *)_pattern.c_str());
}

bool Regex::full_match(const char* str) {
	char* end = NULL;
	bool is_match = partial_match(str, &end);

	return is_match && (*end == '\0');
}

bool Regex::partial_match(const char* str, char** end) {
	int pre = -1;
	const char* pre_end = NULL;
	int cur = _dfa.start();

	while (*str != '\0' && cur >= 0) {
		if (std::find(_dfa.last().begin(), _dfa.last().end(), cur) != _dfa.last().end()) {
			pre = cur;
			pre_end = str;
		}

		const DFATran& tran = _dfa[cur];
		DFATran::const_iterator it = tran.find(*str);
		if (it == tran.end() || it->second == -1) {
			// ����others
			if (it == tran.end()) {
				it = tran.find(OTHER);
			}
			if (it == tran.end() || it->second == -1) {
				cur = -1;
				break;
			}
		}
		cur = it->second;
		str++;
	}

	if (std::find(_dfa.last().begin(), _dfa.last().end(), cur) != _dfa.last().end()) {
		pre = cur;
		pre_end = str;
	}

	if (end != NULL) {
		*end = const_cast<char *>(pre_end);
	}
	return std::find(_dfa.last().begin(), _dfa.last().end(), pre) != _dfa.last().end();
}

} // namespace detail
} // namespace mpl

#if 1

#include <iostream>
using namespace std;

int main() {
	const char* pattern = "\\N+|\\n";
	cout << "pattern: " << pattern << endl;

	::mpl::detail::Regex re;
	if (!re.parse(pattern)) {
		cout << "pattern parse fail" << endl;
		return -1;
	}

	char str[] = "hello\nworld\r\nnihao\r\nshijie";
	cout << "str     : " << str << endl;

	char* begin = str;
	char* end = str;
	while (*end != '\0') {
		bool is_match = re.partial_match(begin, &end);
		char save = *end;
		*end = '\0';

		if (is_match) {
			cout << "match   : " << begin << endl;
		} else {
			cout << "dismatch: " << begin << endl;
		}
		*end = save;

		begin = end;
	}

	return 0;
}

#endif