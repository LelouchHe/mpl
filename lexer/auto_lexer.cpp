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
	for (std::map<const char *, ::mpl::TokenType>::const_iterator it = ::mpl::TOKEN_RE_KEYS.begin();
			it != ::mpl::TOKEN_RE_KEYS.end(); ++it) {
		_generator.parse((::mpl::lexer::detail::Byte *)it->first, (int)it->second);
		_merger.add(_generator);
	}

	for (std::map<const char *, ::mpl::TokenType>::const_iterator it = ::mpl::TOKEN_RE_SYMBOLS.begin();
		it != ::mpl::TOKEN_RE_SYMBOLS.end(); ++it) {
		_generator.parse((::mpl::lexer::detail::Byte *)it->first, (int)it->second);
		_merger.add(_generator);
	}

	_generator.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_ID, (int)::mpl::TT_ID);
	_merger.add(_generator);

	_generator.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_NUMBER, (int)::mpl::TT_NUMBER);
	_merger.add(_generator);

	_generator.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_STRING, (int)::mpl::TT_STRING);
	_merger.add(_generator);

	_generator.parse((::mpl::lexer::detail::Byte *)::mpl::TOKEN_RE_COMMENT, (int)::mpl::TT_COMMENT);
	_merger.add(_generator);

	// 二者好像结果一样,应该需要再看看
	_merger.build();
	_generator.build(_merger, true);
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

::mpl::TokenType AutoLexer::lex() {
	::mpl::lexer::detail::DFA& dfa = _generator;
	_buff.str("");

	const ::mpl::lexer::detail::States& last = _generator.last();
	int pre = -1;
	int cur = dfa.start();

	if (_current == '\0') {
		_current = _reader.next();
	}
	while (!_reader.eof()) {
		if (last.find(cur) != last.end()) {
			pre = cur;
		}

		const ::mpl::lexer::detail::DFATran& tran = dfa[cur];
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
		_buff << _current;

		_current = _reader.next();
	}

	if (last.find(cur) != last.end()) {
		pre = cur;
	}

	if (pre == -1) {
		return ::mpl::TT_EOS;
	} else {
		return token_type(_generator.tags(pre));
	}
}

// 因为基本上mpl词法是LL(1),所以不需要大的缓冲
const ::mpl::Token& AutoLexer::next() {
	if (_ahead.type != ::mpl::TT_EOS) {
		_next = _ahead;
		_ahead.type = ::mpl::TT_EOS;
		return _next;
	}

	_next.type = lex();
	_next.text = _buff.str();

	return _next;
}

const ::mpl::Token& AutoLexer::lookahead() {
	if (_ahead.type == ::mpl::TT_EOS) {
		_ahead.type = lex();
		_ahead.text = _buff.str();
	}

	return _ahead;
}

} // namespace lexer
} // namepsace mpl

#if 1

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