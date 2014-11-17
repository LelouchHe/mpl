#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#include <string>
#include <sstream>
#include <map>

namespace mpl {

class Reader;

// ���ַ�tokenҲ�������Ͱ�
// ��������,���ұ�֤���Ͱ�ȫ
enum TokenType {
	// �ؼ���
	TT_AND, TT_BREAK, TT_DO, TT_ELSE, TT_ELSEIF, TT_END,
	TT_FALSE, TT_FOR, TT_FUNCTION, TT_GOTO, TT_IF, TT_IN,
	TT_LOCAL, TT_NIL, TT_NOT, TT_OR, TT_REPEAT, TT_RETURN,
	TT_THEN, TT_TRUE, TT_UNTIL, TT_WHILE,

	// ����
	TT_LEFT_PARENTHESIS, TT_RIGHT_PARENTHESIS,
	TT_LEFT_SQUARE, TT_RIGHT_SQUARE,
	TT_LEFT_BRACE, TT_RIGHT_BRACE,
	TT_EXP, TT_MUL, TT_DIV, TT_MOD, TT_PLUS, TT_MINUS,
	TT_LESS, TT_LESS_EQUAL, TT_GREATER, TT_GREATER_EQUAL,
	TT_EQUAL, TT_NOT_EQUAL, TT_ASSIGN,
	TT_LEN, TT_COMMA, TT_SEMICOLON,
	TT_COLON, TT_LABEL,
	TT_DOT, TT_CONCAT, TT_VARARG,
	
	// ͳ��
	TT_ID, TT_NUMBER, TT_STRING, TT_COMMENT,

	TT_EOS
};

struct Token {
	TokenType type;
	std::string text;
	Token(): type(TT_EOS) {
	}
};

class Lexer {
public:
	Lexer(::mpl::Reader& reader);
	~Lexer();

	Token next();
	Token lookahead();

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

	static std::map<std::string, TokenType> _s_reserved_tokens;
};

} // namespace mpl

#endif // MPL_LEXER_H