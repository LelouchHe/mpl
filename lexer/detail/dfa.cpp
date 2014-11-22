#include "dfa.h"

#include <iostream>

using namespace std;

namespace mpl {
namespace lexer {
namespace detail {

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

void print_dfa(const DFA& dfa) {
	cout << "start  : " << dfa.start() << endl;
	cout << "last   : ";
	const ::mpl::lexer::detail::States& last = dfa.last();
	print_set(last);
	cout << endl;

	for (size_t i = 0; i < dfa.size(); i++) {
		const ::mpl::lexer::detail::DFATran& tran = dfa[i];
		for (::mpl::lexer::detail::DFATran::const_iterator it = tran.begin();
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
			cout << it->second;
			cout << endl;
		}
	}

	for (::mpl::lexer::detail::States::const_iterator it = last.begin();
		it != last.end(); ++it) {
		const ::mpl::lexer::detail::Tag& tag = dfa.tags(*it);

		cout << *it << ": ";
		print_set(tag);
		cout << endl;
	}
}

} // namespace detail
} // namespace lexer
} // namespace mpl