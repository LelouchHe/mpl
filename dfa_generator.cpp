#include "dfa_generator.h"

#include <set>
#include <algorithm>
#include "dfa_converter.h"

namespace mpl {

typedef std::vector<int> StateList;

DFAGenerator::DFAGenerator() : _start(-1) {

}

DFAGenerator::~DFAGenerator() {

}

int DFAGenerator::new_state() {
	_trans.resize(_trans.size() + 1);
	return _trans.size() - 1;
}

static void init(const ::mpl::DFAConverter& dfa, std::set<StateList> *s) {
	const StateList& last = dfa.last();
	s->insert(last);

	StateList non_last;
	for (size_t i = 0; i < dfa.size(); i++) {
		if (std::find(last.begin(), last.end(), i) == last.end()) {
			non_last.push_back(i);
		}
	}
	s->insert(non_last);
}

static void split(const ::mpl::DFAConverter& dfa,
		const std::vector<const StateList* >& states,
		const StateList& s, std::set<StateList>* t) {
	std::set<char> chars;
	for (size_t i = 0; i < s.size(); i++) {
		const ::mpl::DFAConverter::DFATran& tran = dfa[s[i]];
		for (::mpl::DFAConverter::DFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			chars.insert(it->first);
		}
	}
	
	StateList to_list;
	StateList not_to_list;
	for (std::set<char>::iterator it = chars.begin();
			it != chars.end(); ++it) {
		const StateList* to = NULL;
		to_list.clear();
		not_to_list.clear();
		for (size_t i = 0; i < s.size(); i++) {
			const ::mpl::DFAConverter::DFATran& tran = dfa[s[i]];
			::mpl::DFAConverter::DFATran::const_iterator cit = tran.find(*it);
			if (cit != tran.end()) {
				if (to == NULL) {
					to = states[cit->second];
				}

				if (states[cit->second] == to) {
					to_list.push_back(s[i]);
				} else {
					not_to_list.push_back(s[i]);
				}
			} else {
				not_to_list.push_back(s[i]);
			}
		}

		if (!to_list.empty() && !not_to_list.empty()) {
			t->insert(to_list);
			t->insert(not_to_list);
			return;
		}
	}

	t->insert(s);
}

static void split_all(const ::mpl::DFAConverter& dfa, std::set<StateList>* t) {
	std::vector<const StateList*> states(dfa.size());
	std::set<StateList> p;
	init(dfa, t);

	while (p != *t) {
		p.swap(*t);
		for (std::set<StateList>::iterator it = p.begin(); it != p.end(); ++it) {
			for (size_t i = 0; i < it->size(); i++) {
				states[(*it)[i]] = &(*it);
			}
		}
		t->clear();

		for (std::set<StateList>::iterator it = p.begin();
			it != p.end(); ++it) {
			split(dfa, states, *it, t);
		}
	}
}

void DFAGenerator::build(const ::mpl::DFAConverter& dfa, const std::set<std::vector<int> >& t) {
	const StateList& last = dfa.last();
	std::map<int, int> ms;
	for (std::set<StateList>::iterator it = t.begin();
		it != t.end(); ++it) {
		int s = new_state();
		bool is_last = false;
		for (size_t i = 0; i < it->size(); i++) {
			ms[(*it)[i]] = s;
			if ((*it)[i] == dfa.start()) {
				_start = s;
			} else if (std::find(last.begin(), last.end(), (*it)[i]) != last.end()) {
				if (std::find(_last.begin(), _last.end(), s) == _last.end()) {
					_last.push_back(s);
				}
			}
		}
	}

	for (std::set<StateList>::iterator it = t.begin();
		it != t.end(); ++it) {
		int s = (*it)[0];
		const ::mpl::DFAConverter::DFATran& tran = dfa[s];
		for (::mpl::DFAConverter::DFATran::const_iterator cit = tran.begin();
			cit != tran.end(); ++cit) {
			_trans[ms[s]][cit->first] = ms[cit->second];
		}
	}
}

bool DFAGenerator::parse(const char* str) {
	reset();

	::mpl::DFAConverter dfa;
	if (!dfa.parse(str)) {
		return false;
	}

	std::set<StateList> t;
	split_all(dfa, &t);

	build(dfa, t);

	return true;
}
void DFAGenerator::reset() {
	_trans.clear();
	_start = -1;
	_last.clear();
}

const DFAGenerator::DFATran& DFAGenerator::operator[](size_t s) const {
	return _trans[s];
}

size_t DFAGenerator::size() const {
	return _trans.size();
}

int DFAGenerator::start() const {
	return _start;
}

const std::vector<int>& DFAGenerator::last() const {
	return _last;
}

bool DFAGenerator::match(const char* str) const {
	int cur = _start;
	while (*str != '\0') {
		const ::mpl::DFAGenerator::DFATran& tran = _trans[cur];
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
	const char* pattern = "a[0-b]?";
	::mpl::DFAGenerator dfa;
	dfa.parse(pattern);

	cout << "pattern: " << pattern << endl;
	cout << "start  : " << dfa.start() << endl;
	cout << "last   : ";
	print_vector(dfa.last());
	cout << endl;

	for (size_t i = 0; i < dfa.size(); i++) {
		const ::mpl::DFAGenerator::DFATran& tran = dfa[i];
		for (::mpl::DFAGenerator::DFATran::const_iterator it = tran.begin();
			it != tran.end(); ++it) {
			cout << i << "(" << it->first << ")";
			cout << "\t->\t";
			cout << it->second;
			cout << endl;
		}
	}

	const char* str = "ab";
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