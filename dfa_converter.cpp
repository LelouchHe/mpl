#include "dfa_converter.h"

#include <queue>
#include <set>
#include <algorithm>
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

// �����ܴ�fromͨ��'\0'�ܴﵽ��NFA״̬(����from)
static std::vector<int> fill(const NFAConverter& nfa, const std::vector<int>& from, bool* is_last) {
	std::vector<int> to;
	std::set<int> dedup;
	for (size_t i = 0; i < from.size(); i++) {
		to.push_back(from[i]);
		dedup.insert(from[i]);
	}

	for (size_t i = 0; i < to.size(); i++) {
		const ::mpl::NFAConverter::NFATran& trans = nfa[to[i]];
		::mpl::NFAConverter::NFATran::const_iterator it = trans.find('\0');
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

	*is_last = dedup.find(nfa.last()) != dedup.end();

	return to;
}

// ���д�fromͨ��һ��ch�����NFA״̬
static std::vector<int> expand(const NFAConverter& nfa, const std::vector<int>& from, char ch) {
	std::vector<int> to;
	std::set<int> dedup;
	for (size_t i = 0; i < from.size(); i++) {
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

	std::map<::mpl::NFAConverter::StateList, size_t> nfa_to_dfa;
	bool is_last = false;

	std::queue<::mpl::NFAConverter::StateList> q;
	::mpl::NFAConverter::StateList v;
	v.push_back(nfa.start());
	v = fill(nfa, v, &is_last);
	q.push(v);

	nfa_to_dfa[v] = new_state();
	_start = 0;
	if (is_last) {
		_last.push_back(0);
	}

	while (!q.empty()) {
		v = q.front();
		q.pop();

		int from = nfa_to_dfa[v];

		for (size_t i = 0; i < v.size(); i++) {
			const ::mpl::NFAConverter::NFATran& tran = nfa[v[i]];
			for (::mpl::NFAConverter::NFATran::const_iterator it = tran.begin();
					it != tran.end(); ++it) {
				if (it->first == '\0') {
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

bool DFAConverter::match(const char* str) const {
	int cur = _start;
	while (*str != '\0') {
		const ::mpl::DFAConverter::DFATran& tran = _trans[cur];
		::mpl::DFAConverter::DFATran::const_iterator it = tran.find(*str);
		if (it == tran.end()) {
			cur = -1;
			break;
		}

		cur = it->second;
		str++;
	}

	return std::find(_last.begin(), _last.end(), cur) != _last.end();
}

} // namespace mpl

#if 1

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
	const char* pattern = "[^a]";
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
			cout << i << "(" << it->first << ")";
			cout << "\t->\t";
			cout << it->second;
			cout << endl;
		}
	}

	const char* str = "abbcbccbbc";
	cout << str << endl;
	if (dfa.match(str)) {
		cout << "match";
	} else {
		cout << "dismatch";
	}
	cout << endl;

	return 0;
}

#endif