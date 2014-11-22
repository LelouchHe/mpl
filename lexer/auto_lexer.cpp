#include "auto_lexer.h"

#include <algorithm>
#include "../reader.h"
#include "detail/dfa_merger.h"

namespace mpl {
namespace lexer {

AutoLexer::AutoLexer(::mpl::Reader& reader) :
		_reader(reader), _current('\0') {
	init();
}

AutoLexer::~AutoLexer() {

}

void AutoLexer::init() {
	::mpl::lexer::detail::DFAMerger merger;

	for (std::map<const char *, ::mpl::TokenType>::const_iterator it = ::mpl::TOKEN_RE_KEYS.begin();
			it != ::mpl::TOKEN_RE_KEYS.end(); ++it) {
		_dfa.parse((::mpl::lexer::detail::Byte *)it->first, (int)it->second);
		merger.add(_dfa);
	}

	for (std::map<const char *, ::mpl::TokenType>::const_iterator it = ::mpl::TOKEN_RE_SYMBOLS.begin();
		it != ::mpl::TOKEN_RE_SYMBOLS.end(); ++it) {
		_dfa.parse((::mpl::lexer::detail::Byte *)it->first, (int)it->second);
		merger.add(_dfa);
	}

	_dfa.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_ID, (int)::mpl::TT_ID);
	merger.add(_dfa);
	/*
	_dfa.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_NUMBER, (int)::mpl::TT_NUMBER);
	merger.add(_dfa);

	_dfa.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_STRING, (int)::mpl::TT_STRING);
	merger.add(_dfa);

	_dfa.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_COMMENT, (int)::mpl::TT_COMMENT);
	merger.add(_dfa);
	*/

	merger.build();

	_dfa.build(merger);
}

::mpl::TokenType token_type(const ::mpl::lexer::detail::Tag& tag) {
	int min = (int)::mpl::TT_EOS;
	for (::mpl::lexer::detail::Tag::const_iterator it = tag.begin();
			it != tag.end(); ++it) {
		if (min > *it) {
			min = *it;
		}
	}

	return (::mpl::TokenType)min;
}

const ::mpl::Token& AutoLexer::next() {
	const ::mpl::lexer::detail::StateList& last = _dfa.last();
	int pre = -1;
	int cur = _dfa.start();

	if (_current == '\0') {
		_current = _reader.next();
	}
	while (!_reader.eof()) {
		if (std::find(last.begin(), last.end(), cur) != last.end()) {
			pre = cur;
		}

		const ::mpl::lexer::detail::DFATran& tran = _dfa[cur];
		::mpl::lexer::detail::DFATran::const_iterator it = tran.find(_current);
		
		if (it == tran.end() || it->second == -1) {
			if (it == tran.end()) {
				it = tran.find(::mpl::lexer::detail::OTHER);
			}

			if (it == tran.end() || it->second == -1) {
				cur = -1;
				break;
			}
		}
		cur = it->second;
		_current = _reader.next();
	}

	if (std::find(_dfa.last().begin(), _dfa.last().end(), cur) != _dfa.last().end()) {
		pre = cur;
	}

	if (pre == -1) {
		_next.type = ::mpl::TT_EOS;
	} else {
		_next.type = token_type(_dfa.tags(pre));
	}

	return _next;
}

const ::mpl::Token& AutoLexer::lookahead() {
	return _ahead;
}

} // namespace lexer
} // namepsace mpl

#if 0

#include <iostream>
#include "../file_reader.h"

static void print_token(const ::mpl::Token& t) {
	std::cout << t.type << "\t" << t.text << std::endl;
}

int main() {
	::mpl::FileReader fr("test.txt");

	::mpl::lexer::AutoLexer lexer(fr);

	while (true) {
		::mpl::Token t = lexer.next();
		if (t.type == ::mpl::TT_EOS) {
			break;
		}

		print_token(t);
	}
}

#endif