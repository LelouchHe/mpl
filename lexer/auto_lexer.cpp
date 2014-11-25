#include "auto_lexer.h"

#include "../reader.h"

namespace mpl {
namespace lexer {

AutoLexer::AutoLexer(::mpl::Reader& reader) :
		_reader(reader), _current('\0') {
	init();
}

AutoLexer::~AutoLexer() {

}

void AutoLexer::init() {
	for (std::map<const char *, TokenType>::const_iterator it = TOKEN_RE_KEYS.begin();
			it != TOKEN_RE_KEYS.end(); ++it) {
		_generator.parse((::mpl::lexer::detail::Byte *)it->first, (int)it->second);
		_merger.add(_generator);
	}

	for (std::map<const char *, TokenType>::const_iterator it = TOKEN_RE_SYMBOLS.begin();
		it != TOKEN_RE_SYMBOLS.end(); ++it) {
		_generator.parse((::mpl::lexer::detail::Byte *)it->first, (int)it->second);
		_merger.add(_generator);
	}

	_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_ID, (int)TT_ID);
	_merger.add(_generator);

	_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_NUMBER, (int)TT_NUMBER);
	_merger.add(_generator);

	_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_STRING, (int)TT_STRING);
	_merger.add(_generator);

	_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_COMMENT, (int)TT_COMMENT);
	_merger.add(_generator);

	// 二者好像结果一样,应该需要再看看
	_merger.build();
	_generator.build(_merger, true);
}

TokenType token_type(const ::mpl::lexer::detail::Tag& tag) {
	int min = (int)EOS;
	for (::mpl::lexer::detail::Tag::const_iterator it = tag.begin();
			it != tag.end(); ++it) {
		if (min > *it) {
			min = *it;
		}
	}

	return (TokenType)min;
}

// 因为基本上mpl词法是LL(1),所以不需要大的缓冲
TokenType AutoLexer::lex() {
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
			return TokenType::EOS;
		} else {
			return TokenType::ERROR;
		}
	}

	TokenType type = token_type(_generator.tags(pre));
	// 跳过空格和换行
	if (type == TokenType::TT_SPACE || type == TokenType::TT_NEWLINE) {
		return lex();
	} else {
		return type;
	}
}

const AutoLexer::Token& AutoLexer::next() {
	if (_ahead.type != TokenType::EOS) {
		_next = _ahead;
		_ahead.type = TokenType::EOS;
		return _next;
	}

	_next.type = lex();
	_next.text = _buf.str();

	return _next;
}

const AutoLexer::Token& AutoLexer::lookahead() {
	if (_ahead.type == TokenType::EOS) {
		_ahead.type = lex();
		_ahead.text = _buf.str();
	}

	return _ahead;
}

AutoLexer::Token AutoLexer::parse(const char* begin, char** end) {
	::mpl::lexer::detail::DFA& dfa = _generator;
	const ::mpl::lexer::detail::States& last = _generator.last();

	std::ostringstream buf;
	const char* current = begin;

	Token t;
	while (*current != '\0') {
		buf.str("");
		int pre = -1;
		const char* pre_end = NULL;
		int cur = dfa.start();

		while (*current != '\0') {
			if (last.find(cur) != last.end()) {
				pre = cur;
				pre_end = current;
			}

			const ::mpl::lexer::detail::DFATran& tran = dfa[cur];
			::mpl::lexer::detail::DFATran::const_iterator it = tran.find(*current);

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
			buf << _current;
			current++;
		}

		if (last.find(cur) != last.end()) {
			pre = cur;
			pre_end = current;
		}


		t.text = buf.str();

		TokenType type;
		if (pre == -1) {
			if (*current == '\0') {
				type = TokenType::EOS;
			} else {
				type = TokenType::ERROR;
			}
		} else {
			type = token_type(_generator.tags(pre));
		}

		if (type != TokenType::TT_SPACE && type != TokenType::TT_NEWLINE) {
			t.type = type;
			if (end != NULL) {
				*end = const_cast<char *>(pre_end);
			}
			break;
		}
	}

	return t;
}

} // namespace lexer
} // namepsace mpl

#if 0

#include <iostream>
#include "../file_reader.h"

static void print_token(const ::mpl::lexer::AutoLexer::Token& t) {
	std::cout << t.type << "\t" << t.text << std::endl;
}

int main() {
	::mpl::FileReader fr("test.txt");

	::mpl::lexer::AutoLexer lexer(fr);

	while (true) {
		const ::mpl::lexer::AutoLexer::Token& t = lexer.next();
		if (t.type == ::mpl::lexer::AutoLexer::TokenType::EOS) {
			break;
		}

		print_token(t);
	}
}

#endif