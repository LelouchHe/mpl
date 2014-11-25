#ifndef MPL_LEXER_GeneratedLexer_H
#define MPL_LEXER_GeneratedLexer_H
#include <cassert>
#include <sstream>
#include "detail/state.h"
namespace mpl {
class Reader;
namespace lexer {
class GeneratedLexer {
public:
    GeneratedLexer(::mpl::Reader& reader);
    ~GeneratedLexer();
public:
	enum TokenType {
		TT_AND = 0, TT_BREAK, TT_DO, TT_ELSE, TT_END, 
		TT_FALSE, TT_FOR, TT_FUNCTION, TT_GOTO, TT_IF, TT_IN, 
		TT_LOCAL, TT_NIL, TT_NOT, TT_OR, TT_REPEAT, TT_RETURN, 
		TT_THEN, TT_TRUE, TT_UNTIL, TT_WHILE, TT_LEFT_PARENTHESIS, TT_RIGHT_PARENTHESIS, 
		TT_LEFT_SQUARE, TT_RIGHT_SQUARE, TT_LEFT_BRACE, TT_RIGHT_BRACE, TT_EXP, TT_MUL, 
		TT_DIV, TT_MOD, TT_PLUS, TT_MINUS, TT_LESS, TT_LESS_EQUAL, 
		TT_GREATER, TT_GREATER_EQUAL, TT_EQUAL, TT_NOT_EQUAL, TT_ASSIGN, TT_LEN, 
		TT_COMMA, TT_SEMICOLON, TT_COLON, TT_LABEL, TT_DOT, TT_CONCAT, 
		TT_VARARG, TT_SPACE, TT_NEWLINE, TT_ID, TT_NUMBER, TT_STRING, 
		TT_COMMENT, 
		EOS, ERROR, SKIP, LAST_TOKEN
	};
	class Token {
	public:
	    TokenType type;
	    std::string text;
	    Token() : type(EOS) {}
	};
    bool parse();
    const Token& next();
    const Token& lookahead();
public:
    int line_num;
public:
    void NEWLINE_action(Token& token);
    void SPACE_action(Token& token);
private:
    TokenType lex();
    TokenType token_type(int tag);
    ::mpl::Reader& _reader;
    std::ostringstream _buf;
    char _current;
    Token _next;
    Token _ahead;
};
} // namespace lexer
} // namespace mpl
#endif // MPL_LEXER_GeneratedLexer_H
