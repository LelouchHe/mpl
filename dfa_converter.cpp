#include "dfa_converter.h"

#include <queue>
#include <set>
#include <algorithm>
#include <cassert>
#include "nfa_converter.h"

namespace mpl {

DFAConverter::DFAConverter() : _start(-1) {

}

DFAConverter::~DFAConverter() {

}

int DFAConverter::new_state() {
	_trans.resize(_trans.size() + 1);
	return _trans.size() - 1;
}

// 所有能从from通过'\0'能达到的NFA状态(包括from)
static std::vector<int> fill(const NFAConverter& nfa, const std::vector<int>& from, bool* is_last) {
	std::vector<int> to;
	std::set<int> dedup;
	for (size_t i = 0; i < from.size(); i++) {
		to.push_back(from[i]);
		dedup.insert(from[i]);
	}

	std::set<int> ends;
	for (size_t i = 0; i < to.size(); i++) {
		// -1: 显式错误情况,一般用于[^]
		if (to[i] == -1) {
			ends.insert(-1);
			continue;
		}
		const ::mpl::NFAConverter::NFATran& trans = nfa[to[i]];
		::mpl::NFAConverter::NFATran::const_iterator it = trans.find('\0');
		if (it == trans.end()) {
			ends.insert(to[i]);
			continue;
		}

		for (size_t j = 0; j < it->second.size(); j++) {
			if (dedup.find(it->second[j]) != dedup.end()) {
				continue;
			}

			to.push_back(it->second[j]);
			dedup.insert(it->second[j]);
		}
	}

	*is_last = dedup.find(nfa.last()) != dedup.end();

	if (ends.find(-1) == ends.end() || ends.size() > 1) {
		to.erase(std::remove(to.begin(), to.end(), -1), to.end());
	} else {
		to.clear();
		to.push_back(-1);
	}

	std::sort(to.begin(), to.end());

	return to;
}

// 所有从from通过一次ch到达的NFA状态
static std::vector<int> expand(const NFAConverter& nfa, const std::vector<int>& from, char ch) {
	std::vector<int> to;
	std::set<int> dedup;
	for (size_t i = 0; i < from.size(); i++) {
		// -1: 显式错误情况
		if (from[i] == -1) {
			to.clear();
			to.push_back(-1);
			break;
		}
		const ::mpl::NFAConverter::NFATran& trans = nfa[from[i]];
		::mpl::NFAConverter::NFATran::const_iterator it = trans.find(ch);
		if (it == trans.end()) {
			continue;
		}

		for (size_t j = 0; j < it->second.size(); j++) {
			if (dedup.find(it->second[j]) != dedup.end()) {
				continue;
			}

			to.push_back(it->second[j]);
			dedup.insert(it->second[j]);
		}
	}

	sort(to.begin(), to.end());
	return to;
}

bool DFAConverter::parse(const char* str) {
	reset();

	NFAConverter nfa;
	if (!nfa.parse(str)) {
		return false;
	}

	std::map<::mpl::NFAConverter::StateList, int> nfa_to_dfa;
	::mpl::NFAConverter::StateList v;
	v.push_back(-1);
	nfa_to_dfa[v] = -1;
	v.clear();
	bool is_last = false;

	v.push_back(nfa.start());
	v = fill(nfa, v, &is_last);
	nfa_to_dfa[v] = new_state();
	_start = 0;
	if (is_last) {
		_last.push_back(0);
	}

	std::queue<::mpl::NFAConverter::StateList> q;

	q.push(v);
	while (!q.empty()) {
		v = q.front();
		q.pop();

		int from = nfa_to_dfa[v];
		std::set<char> dedup;
		dedup.insert('\0');

		for (size_t i = 0; i < v.size(); i++) {
			assert(v[i] >= 0);

			const ::mpl::NFAConverter::NFATran& tran = nfa[v[i]];
			for (::mpl::NFAConverter::NFATran::const_iterator it = tran.begin();
					it != tran.end(); ++it) {
				if (dedup.find(it->first) != dedup.end()) {
					continue;
				}

				is_last = false;
				::mpl::NFAConverter::StateList next = expand(nfa, v, it->first);
				next = fill(nfa, next, &is_last);

				int to = -1;
				if (nfa_to_dfa.find(next) == nfa_to_dfa.end()) {
					to = new_state();
					nfa_to_dfa[next] = to;
					q.push(next);
					if (is_last) {
						_last.push_back(to);
					}
				} else {
					to = nfa_to_dfa[next];
				}

				_trans[from][it->first] = to;
				dedup.insert(it->first);
			}
		}
	}

	return true;
}

void DFAConverter::reset() {
	_trans.clear();
	_start = -1;
	_last.clear();
}

const ::mpl::DFAConverter::DFATran& DFAConverter::operator[](size_t s) const {
	return _trans[s];
}

size_t DFAConverter::size() const {
	return _trans.size();
}

int DFAConverter::start() const {
	return _start;
}

const std::vector<int>& DFAConverter::last() const {
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
	const char* pattern = "[_a-zA-Z][_a-zA-Z0-9]*|[ ]";
	::mpl::DFAConverter dfa;
	dfa.parse(pattern);

	cout << "pattern: " << pattern << endl;
	cout << "start  : " << dfa.start() << endl;
	cout << "last   : ";
	const std::vector<int>& last = dfa.last();
	print_vector(last);
	cout << endl;

	for (size_t i = 0; i < dfa.size(); i++) {
		const ::mpl::DFAConverter::DFATran& tran = dfa[i];
		for (::mpl::DFAConverter::DFATran::const_iterator it = tran.begin();
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
			cout << it->second;
			cout << endl;
		}
	}

	return 0;
}

#endif