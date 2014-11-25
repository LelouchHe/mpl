#ifndef MPL_LEXER_GeneratedLexer_H
#define MPL_LEXER_GeneratedLexer_H
#include <cassert>
#include <sstream>
#include "detail/state.h"
#include <map>
namespace mpl {
class Reader;
namespace lexer {
class GeneratedLexer {
public:
    GeneratedLexer(::mpl::Reader& reader);
    ~GeneratedLexer();
public:
	enum TokenType {
		TT_ASSIGN = 0, TT_COMMENT, TT_DIV, TT_ID, TT_LEFT_PARENTHESIS, 
		TT_MINUS, TT_MUL, TT_NEWLINE, TT_NUMBER, TT_PLUS, 
		TT_RIGHT_PARENTHESIS, TT_SPACE, TT_STRING, 
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
    int num_count;
    int id_count;
public:
    void ID_action(Token& token);
    void NEWLINE_action(Token& token);
    void NUMBER_action(Token& token);
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
