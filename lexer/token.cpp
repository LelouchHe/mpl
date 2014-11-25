#include "token.h"

namespace mpl {
namespace lexer {

const std::map<const char*, TokenType> TOKEN_RE_KEYS = {
	{ "and", TT_AND }, { "break", TT_BREAK }, { "do", TT_DO },
	{ "else", TT_ELSE }, { "end", TT_END }, { "false", TT_FALSE },
	{ "for", TT_FOR }, { "function", TT_FUNCTION }, { "goto", TT_GOTO },
	{ "if", TT_IF }, { "in", TT_IN }, { "local", TT_LOCAL },
	{ "nil", TT_NIL }, { "not", TT_NOT }, { "or", TT_OR },
	{ "repeat", TT_REPEAT }, { "return", TT_RETURN },
	{ "then", TT_THEN }, { "true", TT_TRUE }, { "until", TT_UNTIL },
	{ "while", TT_WHILE }
};

const std::map<const char*, TokenType> TOKEN_RE_SYMBOLS = {
	{ "\\(", TT_LEFT_PARENTHESIS }, { "\\)", TT_RIGHT_PARENTHESIS },
	{ "\\[", TT_LEFT_SQUARE }, { "\\]", TT_RIGHT_SQUARE },
	{ "{", TT_LEFT_BRACE }, { "}", TT_RIGHT_BRACE },
	{ "\\^", TT_EXP }, { "\\*", TT_MUL }, { "/", TT_DIV }, { "%", TT_MOD },
	{ "\\+", TT_PLUS }, { "\\-", TT_MINUS },
	{ "<", TT_LESS }, { "<=", TT_LESS_EQUAL }, { ">", TT_GREATER }, { ">=", TT_GREATER_EQUAL },
	{ "==", TT_EQUAL }, { "~=", TT_NOT_EQUAL }, { "=", TT_ASSIGN },
	{ "#", TT_LEN }, { ",", TT_COMMA }, { ";", TT_SEMICOLON },
	{ ":", TT_COLON }, { "::[_\\a]\\w*::", TT_LABEL },
	{ "\\.", TT_DOT }, { "\\.\\.", TT_CONCAT }, { "\\.\\.\\.", TT_VARARG },
	{ "\\s+", TT_SPACE }, { "\\n", TT_NEWLINE }
};

const char* TOKEN_RE_ID = "[_\\a][\\w]*";
// 目前只处理十进制实数
const char* TOKEN_RE_NUMBER = "\\-?((\\d+)|(\\d*\\.\\d+)([eE][\\-\\+]?\\d+)?)";
// 目前只处理单引号
const char* TOKEN_RE_STRING = "'[^']*'";
// 目前只处理单行注释
const char* TOKEN_RE_COMMENT = "\\-\\-[^\\n]*";

} // namespace lexer
} // namespace mpl