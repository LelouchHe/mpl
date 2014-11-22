#include "dfa_generator.h"

#include <set>
#include <cassert>
#include "dfa_converter.h"

namespace mpl {
namespace lexer {
namespace detail {

DFAGenerator::DFAGenerator() : _start(-1) {

}

DFAGenerator::~DFAGenerator() {

}

int DFAGenerator::new_state() {
	size_t size = _trans.size();
	_trans.resize(size + 1);
	return size;
}

static void init(const DFA& dfa, std::set<States> *s, bool is_sep) {
	const States& last = dfa.last();
	if (is_sep) {
		for (States::const_iterator it = last.begin();
				it != last.end(); ++it) {
			States ss;
			ss.insert(*it);
			s->insert(ss);
		}
	} else {
		s->insert(last);
	}

	States non_last;
	for (size_t i = 0; i < dfa.size(); i++) {
		if (last.find(i) == last.end()) {
			non_last.insert(i);
		}
	}

	if (!non_last.empty()) {
		s->insert(non_last);
	}
}

static void split(const DFA& dfa,
		const std::vector<const States* >& states,
		const States& s, std::set<States>* t) {
	std::set<Byte> chars;
	for (States::const_iterator it = s.begin();
			it != s.end(); ++it) {
		const DFATran& tran = dfa[*it];
		for (DFATran::const_iterator tit = tran.begin();
				tit != tran.end(); ++tit) {
			chars.insert(tit->first);
		}
	}
	
	States to_list;
	States not_to_list;
	for (std::set<Byte>::iterator it = chars.begin();
			it != chars.end(); ++it) {
		const States* to = NULL;
		to_list.clear();
		not_to_list.clear();
		for (States::const_iterator sit = s.begin();
				sit != s.end(); ++sit) {
			const DFATran& tran = dfa[*sit];
			DFATran::const_iterator cit = tran.find(*it);
			// -1错误状态提前判断
			if (cit != tran.end() && cit->second != -1) {
				if (to == NULL) {
					to = states[cit->second];
				}

				if (states[cit->second] == to) {
					to_list.insert(*sit);
				} else {
					not_to_list.insert(*sit);
				}
			} else {
				not_to_list.insert(*sit);
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

static void split_all(const DFA& dfa, std::set<States>* t, bool is_sep) {
	std::vector<const States*> states(dfa.size());
	std::set<States> p;
	init(dfa, t, is_sep);

	// -1状态不做split处理
	while (p != *t) {
		p.swap(*t);
		t->clear();
		for (std::set<States>::iterator it = p.begin(); it != p.end(); ++it) {
			const States& ss = *it;
			for (States::const_iterator sit = ss.begin();
					sit != ss.end(); ++sit) {
				assert(*sit >= 0);
				states[*sit] = &(*it);
			}
		}

		for (std::set<States>::iterator it = p.begin(); it != p.end(); ++it) {
			split(dfa, states, *it, t);
		}
	}
}

static void merge_tags(const DFA& dfa,
		const States& states,
		int s, std::map<size_t, Tag> *tags) {
	size_t size = states.size();
	for (States::const_iterator it = states.begin();
			it != states.end(); ++it) {
		assert(*it >= 0);
		const Tag& t = dfa.tags(*it);
		if (!t.empty()) {
			Tag& tag = (*tags)[s];
			tag.insert(t.begin(), t.end());
		}
	}
}

bool DFAGenerator::build(const DFA& dfa, const std::set<States>& t) {
	const States& last = dfa.last();
	std::map<int, int> ms;
	// 错误状态只有1个,提前写入
	ms[-1] = -1;

	for (std::set<States>::iterator it = t.begin();
			it != t.end(); ++it) {
		int s = new_state();
		bool is_last = false;
		const States& ss = *it;
		for (States::const_iterator sit = ss.begin();
				sit != ss.end(); ++sit) {
			ms[*sit] = s;
			if (*sit == dfa.start()) {
				_start = s;
			} else if (last.find(*sit) != last.end()) {
				_last.insert(s);
			}
		}

		merge_tags(dfa, *it, s, &_tags);
	}

	for (std::set<States>::iterator it = t.begin(); 
			it != t.end(); ++it) {
		int s = *(it->begin());
		const DFATran& tran = dfa[s];
		for (DFATran::const_iterator cit = tran.begin(); 
				cit != tran.end(); ++cit) {
			_trans[ms[s]][cit->first] = ms[cit->second];
		}
	}

	return true;
}

// 最小化还有一个优化,当唯一后续可能状态为-1时,可以直接到-1,后续都不用判断
bool DFAGenerator::build(const DFA& dfa, bool is_sep) {
	reset();

	std::set<States> t;
	split_all(dfa, &t, is_sep);

	return build(dfa, t);
}

bool DFAGenerator::parse(const Byte* str, int tag) {
	DFAConverter dfa;
	if (!dfa.parse(str, tag)) {
		return false;
	}

	return build(dfa, false);
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

const States& DFAGenerator::last() const {
	return _last;
}

} // namespace detail
} // namespace lexer
} // namespace mpl

#if 0

#include <iostream>
using namespace std;

int main() {
	//const char* pattern = "[\\+\\-]?((([0-9]+\\.[0-9]*|\\.[0-9]+)([eE][\\+\\-]?[0-9]+)?)|[0-9]+[eE][\\+\\-]?[0-9]+)";
	const char* pattern = "\\N+|\\n";
	::mpl::lexer::detail::DFAGenerator dfa;
	dfa.parse((const ::mpl::lexer::detail::Byte *)pattern);

	cout << "pattern: " << pattern << endl;
	print_dfa(dfa);

	return 0;
}

#endif