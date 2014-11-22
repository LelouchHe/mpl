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

	// ��Ҫ��ͻ������Ч��
	void save_and_next();
	void nosave_and_next();

	// ��Ҫһ��error���������
	// �ֽ׶�ʹ��assert����֤

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