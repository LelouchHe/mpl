#include "regex.h"
#include <algorithm>

namespace mpl {

Regex::Regex() {

}

Regex::~Regex() {

}

bool Regex::parse(const char* pattern) {
	_pattern.assign(pattern);
	
	return _dfa.parse(_pattern.c_str());
}

bool Regex::full_match(const char* str) {
	char* end = NULL;
	bool is_match = partial_match(str, &end);

	return is_match && (*end == '\0');
}

bool Regex::partial_match(const char* str, char** end) {
	int pre = -1;
	int cur = _dfa.start();

	while (*str != '\0') {
		const ::mpl::DFAGenerator::DFATran& tran = _dfa[cur];
		::mpl::DFAGenerator::DFATran::const_iterator it = tran.find(*str);
		if (it == tran.end()) {
			it = tran.find('\xFF');
			if (it == tran.end()) {
				cur = -1;
				break;
			}
		}

		pre = cur;
		cur = it->second;
		str++;
	}

	if (end != NULL) {
		*end = const_cast<char *>(str);
	}

	return std::find(_dfa.last().begin(), _dfa.last().end(), pre) != _dfa.last().end();
}

} // namespace mpl

#if 1

#include <iostream>
using namespace std;

int main() {
	const char* pattern = "[\\+\\-]?((([0-9]+\\.[0-9]*|\\.[0-9]+)([eE][\\+\\-]?[0-9]+)?)|[0-9]+[eE][\\+\\-]?[0-9]+)";
	cout << "pattern: " << pattern << endl;

	::mpl::Regex re;
	if (!re.parse(pattern)) {
		cout << "pattern parse fail" << endl;
		return -1;
	}

	char str[] = "123.456e100 11";
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

		begin = end + 1;
	}

	if (re.full_match(str)) {
		cout << "match" << endl;
	} else {
		cout << "dismatch" << endl;
	}

	return 0;
}

#endif