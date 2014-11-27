#ifndef MPL_LEXER_TOKEN_H
#define MPL_LEXER_TOKEN_H

#include <string>
#include <map>

namespace mpl {
namespace lexer {

// 单字符token也返回类型吧
// 反正不多,而且保证类型安全
// 越小优先级越高
enum TokenType {
	// 关键字
	TT_AND, TT_BREAK, TT_DO, TT_ELSE, TT_ELSEIF, TT_END,
	TT_FALSE, TT_FOR, TT_FUNCTION, TT_GOTO, TT_IF, TT_IN,
	TT_LOCAL, TT_NIL, TT_NOT, TT_OR, TT_REPEAT, TT_RETURN,
	TT_THEN, TT_TRUE, TT_UNTIL, TT_WHILE,

	// 符号
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

	// 统称
	TT_ID, TT_NUMBER, TT_STRING, TT_COMMENT,

	// 保留token,主要用于控制和与Generator兼容
	EOS, EPSILON, NONTERMINAL, ERROR, SKIP, LAST_TOKEN,
};

extern const std::map<const char*, TokenType> TOKEN_RE_KEYS;
extern const std::map<const char*, TokenType> TOKEN_RE_SYMBOLS;
extern const char* TOKEN_RE_ID;
extern const char* TOKEN_RE_NUMBER;
extern const char* TOKEN_RE_STRING;
extern const char* TOKEN_RE_COMMENT;

class Token{
public:
	typedef TokenType TokenType;

public:
	Token() : type(EOS) {

	}
	Token(TokenType atype, const std::string& atext) : type(atype), text(atext) {

	}

public:
	TokenType type;
	std::string text;
};

inline bool operator==(const Token& left, const Token& right) {
	if (left.type != right.type) {
		return false;
	}

	return left.text == right.text;
}

inline bool operator<(const Token& left, const Token& right) {
	if (left.type < right.type) {
		return true;
	} else if (left.type > right.type) {
		return false;
	}

	return left.text < right.text;
}

inline bool operator<=(const Token& left, const Token& right) {
	return left < right || left == right;
}

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_TOKEN_H