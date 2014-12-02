#include "token.h"

namespace mpl {
namespace lexer {

const std::map<std::string, TokenType> TOKEN_RE_KEYS = {
	{ "and", TT_AND }, { "break", TT_BREAK }, { "do", TT_DO },
	{ "else", TT_ELSE }, { "end", TT_END }, { "false", TT_FALSE },
	{ "for", TT_FOR }, { "function", TT_FUNCTION }, { "goto", TT_GOTO },
	{ "if", TT_IF }, { "in", TT_IN }, { "local", TT_LOCAL },
	{ "nil", TT_NIL }, { "not", TT_NOT }, { "or", TT_OR },
	{ "repeat", TT_REPEAT }, { "return", TT_RETURN },
	{ "then", TT_THEN }, { "true", TT_TRUE }, { "until", TT_UNTIL },
	{ "while", TT_WHILE }
};

const std::map<std::string, TokenType> TOKEN_RE_SYMBOLS = {
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

// 目前只处理十进制实数
const std::string TOKEN_RE_NUMBER = "\\-?((\\d+)|(\\d*\\.\\d+)([eE][\\-\\+]?\\d+)?)";

const std::string TOKEN_RE_ID = "[_\\a][\\w]*";
// 目前只处理单引号
const std::string TOKEN_RE_STRING = "'[^']*'";
// 目前只处理单行注释
const std::string TOKEN_RE_COMMENT = "\\-\\-[^\\n]*";

const std::map<std::string, TokenType> TOKEN_TYPES = {
	{ "AND", TT_AND }, { "BREAK", TT_BREAK }, { "DO", TT_DO },
	{ "ELSE", TT_ELSE }, { "END", TT_END }, { "FALSE", TT_FALSE },
	{ "FOR", TT_FOR }, { "FUNCTION", TT_FUNCTION }, { "GOTO", TT_GOTO },
	{ "IF", TT_IF }, { "IN", TT_IN }, { "LOCAL", TT_LOCAL },
	{ "NIL", TT_NIL }, { "NOT", TT_NOT }, { "OR", TT_OR },
	{ "REPEAT", TT_REPEAT }, { "RETURN", TT_RETURN },
	{ "THEN", TT_THEN }, { "TRUE", TT_TRUE }, { "UNTIL", TT_UNTIL },
	{ "WHILE", TT_WHILE },

	{ "LEFT_PARENTHESIS", TT_LEFT_PARENTHESIS }, { "RIGHT_PARENTHESIS", TT_RIGHT_PARENTHESIS },
	{ "LEFT_SQUARE", TT_LEFT_SQUARE }, { "RIGHT_SQUARE", TT_RIGHT_SQUARE },
	{ "LEFT_BRACE", TT_LEFT_BRACE }, { "RIGHT_BRACE", TT_RIGHT_BRACE },
	{ "EXP", TT_EXP }, { "MUL", TT_MUL }, { "DIV", TT_DIV }, { "MOD", TT_MOD },
	{ "PLUS", TT_PLUS }, { "MINUS", TT_MINUS },
	{ "LESS", TT_LESS }, { "LESS_EQUAL", TT_LESS_EQUAL }, { "GREATER", TT_GREATER }, { "GREATER_EQUAL", TT_GREATER_EQUAL },
	{ "EQUAL", TT_EQUAL }, { "NOT_EQUAL", TT_NOT_EQUAL }, { "ASSIGN", TT_ASSIGN },
	{ "LEN", TT_LEN }, { "COMMA", TT_COMMA }, { "SEMICOLON", TT_SEMICOLON },
	{ "COLON", TT_COLON }, { "LABEL", TT_LABEL },
	{ "DOT", TT_DOT }, { "CONCAT", TT_CONCAT }, { "VARARG", TT_VARARG },
	{ "SPACE", TT_SPACE }, { "NEWLINE", TT_NEWLINE },

	{ "ID", TT_ID }, { "NUMBER", TT_NUMBER }, { "STRING", TT_STRING }, { "COMMENT", TT_COMMENT },
};

const std::vector<std::string> TOKEN_NAMES = {
	"AND", "BREAK", "DO",
	"ELSE", "END", "FALSE",
	"FOR", "FUNCTION", "GOTO",
	"IF", "IN", "LOCAL",
	"NIL", "NOT", "OR",
	"REPEAT", "RETURN", 
	"THEN", "TRUE", "UNTIL",
	"WHILE",

	"LEFT_PARENTHESIS", "RIGHT_PARENTHESIS",
	"LEFT_SQUARE", "RIGHT_SQUARE", 
	"LEFT_BRACE", "RIGHT_BRACE", 
	"EXP", "MUL", "DIV", "MOD", 
	"PLUS", "MINUS",
	"LESS", "LESS_EQUAL", "GREATER", "GREATER_EQUAL", 
	"EQUAL", "NOT_EQUAL", "ASSIGN", 
	"LEN", "COMMA", "SEMICOLON", 
	"COLON", "LABEL",
	"DOT", "CONCAT", "VARARG", 
	"SPACE", "NEWLINE",

	"ID", "NUMBER", "STRING", "COMMENT", 

	"$", "", "NONTERMINAL", "ERROR", "SKIP", "LAST_TOKEN",
};

} // namespace lexer
} // namespace mpl