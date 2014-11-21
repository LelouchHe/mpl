#include "dfa_merger.h"
#include <cassert>

namespace mpl {
namespace detail {

DFAMerger::DFAMerger() : _start(-1), _last(-1) {

}

DFAMerger::~DFAMerger() {

}

int DFAMerger::new_state() {
	size_t size = _trans.size();
	_trans.resize(size + 1);
	return size;
}

bool DFAMerger::add(const DFA& dfa) {
	if (_start == -1) {
		_start = new_state();
	}
	assert(_start >= 0);
	if (_last == -1) {
		_last = new_state();
	}
	assert(_last >= 0);

	size_t size = dfa.size();
	size_t base = _trans.size();
	_trans.resize(base + size);

	for (size_t i = 0; i < size; i++) {
		const DFATran& tran = dfa[i];
		for (DFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			_trans[base + i][it->first].push_back(base + it->second);
		}
		const Tag& tag = dfa.tags(i);
		if (!tag.empty()) {
			Tag& to = _tags[base + i];
			to.insert(to.end(), tag.begin(), tag.end());
		}
	}


	_trans[_start][EPSILON].push_back(base + dfa.start());

	const StateList& last = dfa.last();
	for (size_t i = 0; i < last.size(); i++) {
		_trans[base + last[i]][EPSILON].push_back(_last);
	}

	return true;
}

bool DFAMerger::build() {
	assert(_start >= 0 && _last >= 0);
	return _converter.build(_start, _last, _trans, _tags);
}

size_t DFAMerger::size() const {
	return _converter.size();
}

const DFATran& DFAMerger::operator[](size_t s) const {
	return _converter[s];
}

const Tag& DFAMerger::tags(size_t s) const {
	return _converter.tags(s);
}

int DFAMerger::start() const {
	return _converter.start();
}

const StateList& DFAMerger::last() const {
	return _converter.last();
}

} // namespace detail
} // namespace mpl

#if 0

#include <iostream>
#include "dfa_generator.h"
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
	const char* pattern = "[_a-zA-Z][_a-zA-Z0-9]*|[ ]+";
	::mpl::detail::DFAGenerator gen;
	::mpl::detail::DFAMerger merger;

	gen.parse((const ::mpl::detail::Byte *)"[_a-zA-Z][_a-zA-Z0-9]*", 1);
	merger.add(gen);

	gen.parse((const ::mpl::detail::Byte *)" +", 2);
	merger.add(gen);

	gen.parse((const ::mpl::detail::Byte *)"[1-9][0-9]*", 3);
	merger.add(gen);

	merger.build();
	gen.build(merger);

	cout << "start  : " << gen.start() << endl;
	cout << "last   : ";
	const std::vector<int>& last = gen.last();
	print_vector(last);
	cout << endl;

	for (size_t i = 0; i < gen.size(); i++) {
		const ::mpl::detail::DFATran& tran = gen[i];
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
		const ::mpl::detail::Tag& tag = gen.tags(last[i]);

		cout << last[i] << ": ";
		print_vector(tag);
		cout << endl;
	}

	return 0;
}

#endif