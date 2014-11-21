#ifndef MPL_LEXER_MANUAL_LEXER_H
#define MPL_LEXER_MANUAL_LEXER_H

#include <map>
#include <sstream>
#include "../lexer.h"
#include "../reader.h"

namespace mpl {
namespace lexer {

class Reader;

class ManualLexer : public ::mpl::Lexer {
public:
	ManualLexer(::mpl::Reader& reader);
	~ManualLexer();

public:
	virtual const ::mpl::Token& next();
	virtual const ::mpl::Token& lookahead();

private:
	TokenType lex();

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
	::mpl::Reader& _reader;

	std::ostringstream _buff;
	char _current;

	int _line_num;

	Token _next;
	Token _ahead;

	static std::map<std::string, ::mpl::TokenType> _s_reserved_tokens;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_MANUAL_LEXER_H