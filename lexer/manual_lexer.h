#ifndef MPL_LEXER_MANUAL_LEXER_H
#define MPL_LEXER_MANUAL_LEXER_H

#include <map>
#include <sstream>
#include "token.h"

namespace mpl {
class Reader;

namespace lexer {

class ManualLexer {
public:
	ManualLexer(::mpl::Reader& reader);
	~ManualLexer();

public:
	typedef Token Token;
	typedef TokenType TokenType;

	const Token& next();
	const Token& lookahead();

	static TokenType token_type(const std::string& name);
	static const std::string& token_name(TokenType type);

private:
	TokenType lex();

	// 主要是突出语义效果
	void save_and_next();
	void nosave_and_next();

	// 需要一个error来方便调试
	// 现阶段使用assert来保证

	void new_line(bool should_save);
	void escape();
	int count_nosave(char ch);

	void read_id();
	void read_number();
	void read_string();
	void read_long_string(int sep);
	void read_comment();

	bool eof();

private:
	::mpl::Reader& _reader;
	std::ostringstream _buf;
	char _current;

	int _line_num;

	Token _next;
	Token _ahead;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_MANUAL_LEXER_H