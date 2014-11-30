#include "auto_lexer.h"

#include "../reader.h"
#include "detail/dfa_generator.h"
#include "detail/dfa_merger.h"

namespace mpl {
namespace lexer {

static ::mpl::lexer::detail::DFAGenerator s_generator;
static ::mpl::lexer::detail::DFAMerger s_merger;
static bool s_init = false;

static void init() {
	for (std::map<std::string, TokenType>::const_iterator it = TOKEN_RE_KEYS.begin();
			it != TOKEN_RE_KEYS.end(); ++it) {
		s_generator.parse((::mpl::lexer::detail::Byte *)it->first.c_str(), (int)it->second);
		s_merger.add(s_generator);
	}

	for (std::map<std::string, TokenType>::const_iterator it = TOKEN_RE_SYMBOLS.begin();
			it != TOKEN_RE_SYMBOLS.end(); ++it) {
		s_generator.parse((::mpl::lexer::detail::Byte *)it->first.c_str(), (int)it->second);
		s_merger.add(s_generator);
	}

	s_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_ID.c_str(), (int)TT_ID);
	s_merger.add(s_generator);

	s_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_NUMBER.c_str(), (int)TT_NUMBER);
	s_merger.add(s_generator);

	s_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_STRING.c_str(), (int)TT_STRING);
	s_merger.add(s_generator);

	s_generator.parse((::mpl::lexer::detail::Byte *)TOKEN_RE_COMMENT.c_str(), (int)TT_COMMENT);
	s_merger.add(s_generator);

	// 二者好像结果一样,应该需要再看看
	s_merger.build();
	s_generator.build(s_merger, true);
}

AutoLexer::AutoLexer(::mpl::Reader& reader) :
		_reader(reader), _current('\0') {
	// FIXME: 应该用更强的多线程原语来保证
	if (!s_init) {
		init();
		s_init = true;
	}
}

AutoLexer::~AutoLexer() {

}

static AutoLexer::TokenType tag_type(const ::mpl::lexer::detail::Tag& tag) {
	int min = (int)EOS;
	for (::mpl::lexer::detail::Tag::const_iterator it = tag.begin();
			it != tag.end(); ++it) {
		if (min > *it) {
			min = *it;
		}
	}

	return (AutoLexer::TokenType)min;
}

// 因为基本上mpl词法是LL(1),所以不需要大的缓冲
TokenType AutoLexer::lex() {
	::mpl::lexer::detail::DFA& dfa = s_generator;
	_buf.str("");

	const ::mpl::lexer::detail::States& last = s_generator.last();
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
			_buf << "$";
			return TokenType::EOS;
		} else {
			return TokenType::ERROR;
		}
	}

	TokenType type = tag_type(s_generator.tags(pre));
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

AutoLexer::TokenType AutoLexer::token_type(const std::string& name) {
	std::map<std::string, TokenType>::const_iterator it =
			TOKEN_TYPES.find(name);
	if (it != TOKEN_TYPES.end()) {
		return it->second;
	} else {
		return TokenType::ERROR;
	}
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