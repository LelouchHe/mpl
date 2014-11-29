#ifndef MPL_LEXER_GeneratedLexer_H
#define MPL_LEXER_GeneratedLexer_H
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
		TT_FALSE, TT_FOR, TT_FUNCTION, TT_GOTO, TT_IF, 
		TT_IN, TT_LOCAL, TT_NIL, TT_NOT, TT_OR, 
		TT_REPEAT, TT_RETURN, TT_THEN, TT_TRUE, TT_UNTIL, 
		TT_WHILE, TT_LEFT_PARENTHESIS, TT_RIGHT_PARENTHESIS, TT_LEFT_SQUARE, TT_RIGHT_SQUARE, 
		TT_LEFT_BRACE, TT_RIGHT_BRACE, TT_EXP, TT_MUL, TT_DIV, 
		TT_MOD, TT_PLUS, TT_MINUS, TT_LESS, TT_LESS_EQUAL, 
		TT_GREATER, TT_GREATER_EQUAL, TT_EQUAL, TT_NOT_EQUAL, TT_ASSIGN, 
		TT_LEN, TT_COMMA, TT_SEMICOLON, TT_COLON, TT_LABEL, 
		TT_DOT, TT_CONCAT, TT_VARARG, TT_SPACE, TT_NEWLINE, 
		TT_ID, TT_NUMBER, TT_STRING, TT_COMMENT, 
		EOS, EPSILON, NONTERMINAL, ERROR, SKIP, LAST_TOKEN
	};
	class Token {
	public:
	    typedef TokenType TokenType;
	    TokenType type;
	    std::string text;
	    Token() : type(EOS) {}
	    Token(TokenType atype, const std::string& atext) :
	            type(atype), text(atext) {}
	};
    bool run();
    const Token& next();
    const Token& lookahead();
    static TokenType token_type(const std::string& name);
public:
    int line_num;
public:
    void NEWLINE_action(Token& token);
    void SPACE_action(Token& token);
private:
    TokenType lex();
    TokenType tag_type(int tag);
    ::mpl::Reader& _reader;
    std::ostringstream _buf;
    char _current;
    Token _next;
    Token _ahead;
};
inline bool operator==(const GeneratedLexer::Token& a, const GeneratedLexer::Token& b) {
    if (a.type != b.type) {
        return false;
    }
    return a.text == b.text;
}
inline bool operator<(const GeneratedLexer::Token& a, const GeneratedLexer::Token& b) {
    if (a.type < b.type) {
        return true;
    } else if (a.type > b.type) {
        return false;
    }
    return a.text < b.text;
}
inline bool operator<=(const GeneratedLexer::Token& a, const GeneratedLexer::Token& b) {
    return a < b || a == b;
}
} // namespace lexer
} // namespace mpl
#endif // MPL_LEXER_GeneratedLexer_H
