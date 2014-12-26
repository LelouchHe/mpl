#ifndef MPL_PARSER_GeneratedLALRParser_H
#define MPL_PARSER_GeneratedLALRParser_H
#include "../lexer.h"
#include "../ast/parser_node.h"
#include "../ast/all_nodes.h"
#include "../ast/all_actions.h"
namespace mpl {
class Reader;
namespace parser {
class GeneratedLALRParser {
public:
    GeneratedLALRParser(::mpl::Reader& reader);
    ~GeneratedLALRParser();
public:
    typedef ::mpl::Lexer Lexer;
    typedef Lexer::Token Token;
    typedef Token::TokenType TokenType;
    ::mpl::ast::ParserNodePtr build(bool is_debug = false);
private:
    Lexer _lexer;
};
} // namespace parser
} // namespace mpl
#endif // MPL_PARSER_GeneratedLALRParser_H
