#include "auto_lexer.h"

#include "../reader.h"

namespace mpl {
namespace lexer {

AutoLexer::AutoLexer(::mpl::Reader& reader) :
		Lexer(reader), _current('\0') {
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
	int min = (int)::mpl::EOS;
	for (::mpl::lexer::detail::Tag::const_iterator it = tag.begin();
			it != tag.end(); ++it) {
		if (min > *it) {
			min = *it;
		}
	}

	return (::mpl::TokenType)min;
}

// 因为基本上mpl词法是LL(1),所以不需要大的缓冲
::mpl::TokenType AutoLexer::lex() {
	::mpl::lexer::detail::DFA& dfa = _generator;
	_buf.str("");

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
		_buf << _current;

		_current = _reader.next();
	}

	if (last.find(cur) != last.end()) {
		pre = cur;
	}

	if (pre == -1) {
		if (_reader.eof()) {
			return ::mpl::EOS;
		} else {
			return ::mpl::ERROR;
		}
	}

	::mpl::TokenType type = token_type(_generator.tags(pre));
	// 跳过空格和换行
	if (type == ::mpl::TT_SPACE || type == ::mpl::TT_NEWLINE) {
		return lex();
	} else {
		return type;
	}
}

const ::mpl::Token& AutoLexer::next() {
	if (_ahead.type != ::mpl::EOS) {
		_next = _ahead;
		_ahead.type = ::mpl::EOS;
		return _next;
	}

	_next.type = lex();
	_next.text = _buf.str();

	return _next;
}

const ::mpl::Token& AutoLexer::lookahead() {
	if (_ahead.type == ::mpl::EOS) {
		_ahead.type = lex();
		_ahead.text = _buf.str();
	}

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
	::mpl::FileReader fr("mu.lua");

	::mpl::lexer::AutoLexer lexer(fr);

	while (true) {
		::mpl::Token t = lexer.next();
		if (t.type == ::mpl::EOS) {
			break;
		}

		print_token(t);
	}
}

#endif