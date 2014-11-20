#include "dfa_converter.h"

#include <queue>
#include <set>
#include <algorithm>
#include <cassert>
#include "nfa_converter.h"

namespace mpl {
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
static std::vector<int> fill(const std::vector<NFATran>& trans, int last, const std::vector<int>& from, bool* is_last) {
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
		const NFATran& tran = trans[to[i]];
		NFATran::const_iterator it = tran.find(EPSILON);
		if (it == tran.end()) {
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

	*is_last = dedup.find(last) != dedup.end();

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
static std::vector<int> expand(const std::vector<NFATran>& trans, const std::vector<int>& from, char ch) {
	std::vector<int> to;
	std::set<int> dedup;
	for (size_t i = 0; i < from.size(); i++) {
		// -1: 显式错误情况
		if (from[i] == -1) {
			to.clear();
			to.push_back(-1);
			break;
		}
		const NFATran& tran = trans[from[i]];
		NFATran::const_iterator it = tran.find(ch);
		if (it == tran.end()) {
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

static void merge_tags(const StateList& states,
		const std::map<size_t, Tag>& from,
		int s, std::map<size_t, Tag> *to) {
	size_t size = states.size();
	for (size_t i = 0; i < size; i++) {
		assert(states[i] >= 0);
		std::map<size_t, Tag>::const_iterator it = from.find(states[i]);
		if (it != from.end()) {
			Tag& tag = (*to)[s];
			tag.insert(tag.end(), it->second.begin(), it->second.end());
		}
	}
}

bool DFAConverter::parse(const Byte* str) {
	return parse(str, -1);
}

bool DFAConverter::build(int start, int last,
		const std::vector<NFATran>& trans,
		const std::map<size_t, Tag>& tags) {
	reset();

	std::map<StateList, int> nfa_to_dfa;
	StateList v;
	v.push_back(-1);
	nfa_to_dfa[v] = -1;
	v.clear();
	bool is_last = false;

	v.push_back(start);
	v = fill(trans, last, v, &is_last);
	_start = new_state();
	nfa_to_dfa[v] = _start;
	if (is_last) {
		_last.push_back(_start);
	}
	merge_tags(v, tags, _start, &_tags);

	std::queue<StateList> q;
	q.push(v);
	while (!q.empty()) {
		v = q.front();
		q.pop();

		int from = nfa_to_dfa[v];
		std::set<char> dedup;
		dedup.insert(EPSILON);

		for (size_t i = 0; i < v.size(); i++) {
			assert(v[i] >= 0);

			const NFATran& tran = trans[v[i]];
			for (NFATran::const_iterator it = tran.begin();
					it != tran.end(); ++it) {
				if (dedup.find(it->first) != dedup.end()) {
					continue;
				}

				is_last = false;
				StateList next = expand(trans, v, it->first);
				next = fill(trans, last, next, &is_last);

				int to = -1;
				if (nfa_to_dfa.find(next) == nfa_to_dfa.end()) {
					to = new_state();
					nfa_to_dfa[next] = to;
					q.push(next);
					if (is_last) {
						_last.push_back(to);
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

bool DFAConverter::parse(const Byte* str, int tag) {
	NFAConverter nfa;
	if (!nfa.parse(str)) {
		return false;
	}
	std::map<size_t, Tag> tags;
	tags[nfa.last()].push_back(tag);

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

const StateList& DFAConverter::last() const {
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
	const char* pattern = "[_a-zA-Z][_a-zA-Z0-9]*|[ ]";
	::mpl::detail::DFAConverter dfa;
	dfa.parse((::mpl::detail::Byte *)pattern);

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
			if (it->first == ::mpl::detail::EPSILON) {
				cout << "\\0";
			} else if (it->first == ::mpl::detail::OTHER) {
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

		cout << last[i] << ": " << tag[0];
		print_vector(tag);
		cout << endl;
	}

	return 0;
}

#endif