#ifndef MPL_LEXER_MANUAL_LEXER_H
#define MPL_LEXER_MANUAL_LEXER_H

#include <map>
#include <sstream>
#include "../lexer.h"
#include "../token.h"

namespace mpl {
class Reader;

namespace lexer {

class ManualLexer : public ::mpl::Lexer {
public:
	ManualLexer(::mpl::Reader& reader);
	~ManualLexer();

public:
	virtual const ::mpl::Token& next();
	virtual const ::mpl::Token& lookahead();

private:
	::mpl::TokenType lex();

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

private:
	std::ostringstream _buff;
	char _current;

	int _line_num;

	::mpl::Token _next;
	::mpl::Token _ahead;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_MANUAL_LEXER_H