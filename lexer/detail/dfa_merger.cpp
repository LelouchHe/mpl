#include "dfa_merger.h"
#include <cassert>

namespace mpl {
namespace lexer {
namespace detail {

DFAMerger::DFAMerger() {
	reset();
}

DFAMerger::~DFAMerger() {

}

void DFAMerger::reset() {
	_trans.clear();
	_tags.clear();
	_start = -1;
	_last.clear();
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

	size_t size = dfa.size();
	size_t base = _trans.size();
	_trans.resize(base + size);

	for (size_t i = 0; i < size; i++) {
		const DFATran& tran = dfa[i];
		for (DFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			_trans[base + i][it->first].insert(base + it->second);
		}
		const Tag& tag = dfa.tags(i);
		if (!tag.empty()) {
			Tag& to = _tags[base + i];
			to.insert(tag.begin(), tag.end());
		}
	}


	_trans[_start][EPSILON].insert(base + dfa.start());

	const States& last = dfa.last();
	for (States::const_iterator it = last.begin();
		it != last.end(); ++it) {
		_last.insert(base + *it);
	}

	return true;
}

bool DFAMerger::build() {
	assert(_start >= 0);
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

const States& DFAMerger::last() const {
	return _converter.last();
}

} // namespace detail
} // namespace lexer
} // namespace mpl

#if 0

#include <iostream>
#include "dfa_generator.h"

using namespace std;

int main() {
	::mpl::lexer::detail::DFAGenerator gen;
	::mpl::lexer::detail::DFAMerger merger;

	gen.parse((const ::mpl::lexer::detail::Byte *)"and", 0);
	merger.add(gen);

	gen.parse((const ::mpl::lexer::detail::Byte *)"break", 1);
	merger.add(gen);

	gen.parse((const ::mpl::lexer::detail::Byte *)"do", 2);
	merger.add(gen);

	gen.parse((const ::mpl::lexer::detail::Byte *)"\\+", 32);
	merger.add(gen);

	gen.parse((const ::mpl::lexer::detail::Byte *)"ast", 40);
	merger.add(gen);

	gen.parse((const ::mpl::lexer::detail::Byte *)" ", 49);
	merger.add(gen);

	gen.parse((const ::mpl::lexer::detail::Byte *)"a+", 51);
	merger.add(gen);

	// 这样merger和gen基本上都是一样的了
	merger.build();
	print_dfa(merger);

	gen.build(merger, true);
	print_dfa(gen);

	return 0;
}

#endif