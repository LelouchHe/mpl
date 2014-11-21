#ifndef MPL_TOKEN_H
#define MPL_TOKEN_H

#include <string>
#include <map>

namespace mpl {

// ���ַ�tokenҲ�������Ͱ�
// ��������,���ұ�֤���Ͱ�ȫ
enum TokenType {
	TT_EOS = 0,
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
	TT_SPACE, TT_NEWLINE,

	// ͳ��
	TT_ID, TT_NUMBER, TT_STRING, TT_COMMENT
};

extern const std::map<const char*, TokenType> TOKEN_RE_KEYS;
extern const std::map<const char*, TokenType> TOKEN_RE_SYMBOLS;
extern const char* TOKEN_RE_ID;
extern const char* TOKEN_RE_NUMBER;
extern const char* TOKEN_RE_STRING;
extern const char* TOKEN_RE_COMMENT;

class Token{
public:
	Token() : type(TT_EOS) {

	}

public:
	TokenType type;
	std::string text;
};

} // namespace mpl

#endif // MPL_TOKEN_H