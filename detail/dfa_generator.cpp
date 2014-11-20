#include "dfa_generator.h"

#include <set>
#include <algorithm>
#include <cassert>
#include "dfa_converter.h"

namespace mpl {
namespace detail {

typedef std::vector<int> StateList;

DFAGenerator::DFAGenerator() : _start(-1) {

}

DFAGenerator::~DFAGenerator() {

}

int DFAGenerator::new_state() {
	size_t size = _trans.size();
	_trans.resize(size + 1);
	return size;
}

static void init(const DFA& dfa, std::set<StateList> *s) {
	const StateList& last = dfa.last();
	s->insert(last);

	StateList non_last;
	for (size_t i = 0; i < dfa.size(); i++) {
		if (std::find(last.begin(), last.end(), i) == last.end()) {
			non_last.push_back(i);
		}
	}

	if (!non_last.empty()) {
		s->insert(non_last);
	}
}

static void split(const DFA& dfa,
		const std::vector<const StateList* >& states,
		const StateList& s, std::set<StateList>* t) {
	std::set<char> chars;
	for (size_t i = 0; i < s.size(); i++) {
		const DFATran& tran = dfa[s[i]];
		for (DFATran::const_iterator it = tran.begin();
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
			const DFATran& tran = dfa[s[i]];
			DFATran::const_iterator cit = tran.find(*it);
			// -1错误状态提前判断
			if (cit != tran.end() && cit->second != -1) {
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

static void split_all(const DFA& dfa, std::set<StateList>* t) {
	std::vector<const StateList*> states(dfa.size());
	std::set<StateList> p;
	init(dfa, t);

	// -1状态不做split处理
	while (p != *t) {
		p.swap(*t);
		t->clear();
		for (std::set<StateList>::iterator it = p.begin(); it != p.end(); ++it) {
			for (size_t i = 0; i < it->size(); i++) {
				assert((*it)[i] >= 0);
				states[(*it)[i]] = &(*it);
			}
		}

		for (std::set<StateList>::iterator it = p.begin(); it != p.end(); ++it) {
			split(dfa, states, *it, t);
		}
	}
}

static void merge_tags(const DFA& dfa,
		const StateList& states,
		int s, std::map<size_t, Tag> *tags) {
	size_t size = states.size();
	for (size_t i = 0; i < size; i++) {
		assert(states[i] >= 0);
		const Tag& t = dfa.tags(states[i]);
		if (!t.empty()) {
			Tag& tag = (*tags)[s];
			tag.insert(tag.end(), t.begin(), t.end());
		}
	}
}

bool DFAGenerator::build(const DFA& dfa, const std::set<StateList>& t) {
	const StateList& last = dfa.last();
	std::map<int, int> ms;
	// 错误状态只有1个,提前写入
	ms[-1] = -1;

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

		merge_tags(dfa, *it, s, &_tags);
	}

	for (std::set<StateList>::iterator it = t.begin(); 
			it != t.end(); ++it) {
		int s = (*it)[0];
		const DFATran& tran = dfa[s];
		for (DFATran::const_iterator cit = tran.begin(); 
				cit != tran.end(); ++cit) {
			_trans[ms[s]][cit->first] = ms[cit->second];
		}
	}

	return true;
}

bool DFAGenerator::build(const DFA& dfa) {
	reset();

	std::set<StateList> t;
	split_all(dfa, &t);

	return build(dfa, t);
}

bool DFAGenerator::parse(const Byte* str, int tag) {
	DFAConverter dfa;
	if (!dfa.parse(str, tag)) {
		return false;
	}

	return build(dfa);
}

bool DFAGenerator::parse(const Byte* str) {
	return parse(str, -1);
}

void DFAGenerator::reset() {
	_trans.clear();
	_tags.clear();
	_start = -1;
	_last.clear();
}

size_t DFAGenerator::size() const {
	return _trans.size();
}

const DFATran& DFAGenerator::operator[](size_t s) const {
	assert(s < _trans.size());
	return _trans[s];
}

const Tag& DFAGenerator::tags(size_t s) const {
	std::map<size_t, Tag>::const_iterator it = _tags.find(s);
	if (it != _tags.end()) {
		return it->second;
	} else {
		return NONE_TAG;
	}
}

int DFAGenerator::start() const {
	return _start;
}

const StateList& DFAGenerator::last() const {
	return _last;
}

} // namespace detail
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
	//const char* pattern = "[\\+\\-]?((([0-9]+\\.[0-9]*|\\.[0-9]+)([eE][\\+\\-]?[0-9]+)?)|[0-9]+[eE][\\+\\-]?[0-9]+)";
	const char* pattern = "[_a-zA-Z][_a-zA-Z0-9]*|[ ]";
	::mpl::detail::DFAGenerator dfa;
	dfa.parse((const ::mpl::detail::Byte *)pattern);

	cout << "pattern: " << pattern << endl;
	cout << "start  : " << dfa.start() << endl;
	cout << "last   : ";
	const std::vector<int>& last = dfa.last();
	print_vector(last);
	cout << endl;

	for (size_t i = 0; i < dfa.size(); i++) {
		const ::mpl::detail::DFATran& tran = dfa[i];
		for (::mpl::detail::DFATran::const_iterator it = tran.begin();
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

	for (size_t i = 0; i < last.size(); i++) {
		const ::mpl::detail::Tag& tag = dfa.tags(last[i]);

		cout << last[i] << ": ";
		print_vector(tag);
		cout << endl;
	}

	return 0;
}

#endif