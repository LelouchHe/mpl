#ifndef MPL_PARSER_GeneratedLLParser_H
#define MPL_PARSER_GeneratedLLParser_H
#include "../config.h"
namespace mpl {
class Reader;
namespace parser {
class GeneratedLLParser {
public:
    GeneratedLLParser(::mpl::Reader& reader);
    ~GeneratedLLParser();
public:
    typedef ::mpl::Lexer Lexer;
    typedef Lexer::Token Token;
    typedef Token::TokenType TokenType;
    void parse();
private:
    Lexer _lexer;
};
} // namespace parser
} // namespace mpl
#endif // MPL_LEXER_GeneratedLLParser_H
