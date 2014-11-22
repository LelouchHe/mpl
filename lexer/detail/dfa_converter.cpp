#include "dfa_converter.h"

#include <queue>
#include <set>
#include <cassert>
#include "nfa_converter.h"

namespace mpl {
namespace lexer {
namespace detail {

DFAConverter::DFAConverter() : _start(-1) {

}

DFAConverter::~DFAConverter() {

}

int DFAConverter::new_state() {
	size_t size = _trans.size();
	_trans.resize(size + 1);
	return size;
}

// 所有能从from通过EPSILON能达到的NFA状态(包括from)
static States fill(const std::vector<NFATran>& trans, const States& last, const States& from, bool* is_last) {
	std::queue<int> q;
	for (States::const_iterator it = from.begin();
			it != from.end(); ++it) {
		q.push(*it);
	}

	// ends表示终点(即最终状态),要判断这次转移是否只有-1
	States ends;
	States to;
	while (!q.empty()) {
		int s = q.front();
		q.pop();

		to.insert(s);
		if (last.find(s) != last.end()) {
			*is_last = true;
		}

		if (s == -1) {
			ends.insert(-1);
			continue;
		}

		const NFATran& tran = trans[s];
		NFATran::const_iterator it = tran.find(EPSILON);
		if (it == tran.end()) {
			ends.insert(s);
			continue;
		}

		const States& next = it->second;
		for (States::const_iterator nit = next.begin();
				nit != next.end(); ++nit) {
			if (to.find(*nit) == to.end()) {
				q.push(*nit);
			}
		}
	}

	if (ends.find(-1) == ends.end() || ends.size() > 1) {
		to.erase(-1);
	} else {
		to.clear();
		to.insert(-1);
	}

	return to;
}

// 所有从from通过一次ch到达的NFA状态
// 这里有-1,表示上次fill只有一个最终状态,即-1
static States expand(const std::vector<NFATran>& trans, const States& from, char ch) {
	States to;
	for (States::const_iterator it = from.begin();
			it != from.end(); ++it) {
		int s = *it;
		if (s == -1) {
			to.clear();
			to.insert(-1);
			break;
		}

		const NFATran& tran = trans[s];
		NFATran::const_iterator tit = tran.find(ch);
		if (tit == tran.end()) {
			continue;
		}

		const States& next = tit->second;
		for (States::const_iterator nit = next.begin();
			nit != next.end(); ++nit) {
			to.insert(*nit);
		}
	}

	return to;
}

static void merge_tags(const States& states,
		const std::map<size_t, Tag>& from,
		int s, std::map<size_t, Tag> *to) {
	size_t size = states.size();
	for (States::const_iterator it = states.begin();
			it != states.end(); ++it) {
		assert(*it >= 0);
		std::map<size_t, Tag>::const_iterator fit = from.find(*it);
		if (fit != from.end()) {
			Tag& tag = (*to)[s];
			tag.insert(fit->second.begin(), fit->second.end());
		}
	}
}

bool DFAConverter::parse(const Byte* str) {
	return parse(str, -1);
}

bool DFAConverter::build(int start, const States& last,
	const std::vector<NFATran>& trans,
	const std::map<size_t, Tag>& tags) {

	reset();

	std::map<States, int> nfa_to_dfa;
	States v;
	v.insert(-1);
	nfa_to_dfa[v] = -1;
	v.clear();
	bool is_last = false;

	v.insert(start);
	v = fill(trans, last, v, &is_last);
	_start = new_state();
	nfa_to_dfa[v] = _start;
	if (is_last) {
		_last.insert(_start);
	}
	merge_tags(v, tags, _start, &_tags);

	std::queue<States> q;
	q.push(v);
	while (!q.empty()) {
		v = q.front();
		q.pop();

		int from = nfa_to_dfa[v];
		std::set<Byte> dedup;
		dedup.insert(EPSILON);

		for (States::const_iterator it = v.begin();
				it != v.end(); ++it) {
			int s = *it;
			assert(s >= 0);

			const NFATran& tran = trans[s];
			for (NFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
				if (dedup.find(it->first) != dedup.end()) {
					continue;
				}

				is_last = false;
				States next = expand(trans, v, it->first);
				next = fill(trans, last, next, &is_last);

				int to = -1;
				if (nfa_to_dfa.find(next) == nfa_to_dfa.end()) {
					to = new_state();
					nfa_to_dfa[next] = to;
					q.push(next);
					if (is_last) {
						_last.insert(to);
					}
					merge_tags(next, tags, to, &_tags);
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

bool DFAConverter::build(int start, int last,
		const std::vector<NFATran>& trans,
		const std::map<size_t, Tag>& tags) {
	States lasts;
	lasts.insert(last);

	return build(start, lasts, trans, tags);
}

bool DFAConverter::parse(const Byte* str, int tag) {
	NFAConverter nfa;
	if (!nfa.parse(str)) {
		return false;
	}
	std::map<size_t, Tag> tags;
	tags[nfa.last()].insert(tag);

	return build(nfa.start(), nfa.last(), nfa.trans(), tags);
}

void DFAConverter::reset() {
	_trans.clear();
	_tags.clear();
	_start = -1;
	_last.clear();
}

size_t DFAConverter::size() const {
	return _trans.size();
}

const DFATran& DFAConverter::operator[](size_t s) const {
	assert(s < _trans.size());
	return _trans[s];
}

const Tag& DFAConverter::tags(size_t s) const {
	std::map<size_t, Tag>::const_iterator it = _tags.find(s);
	if (it != _tags.end()) {
		return it->second;
	} else {
		return NONE_TAG;
	}
}

int DFAConverter::start() const {
	return _start;
}

const States& DFAConverter::last() const {
	return _last;
}

} // namespace detail
} // namespace lexer
} // namespace mpl

#if 0

#include <iostream>
using namespace std;

int main() {
	const char* pattern = "\\N+|\\n";
	::mpl::lexer::detail::DFAConverter dfa;
	dfa.parse((::mpl::lexer::detail::Byte *)pattern);

	cout << "pattern: " << pattern << endl;
	print_dfa(dfa);

	return 0;
}

#endif