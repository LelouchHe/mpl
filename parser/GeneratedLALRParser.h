#ifndef MPL_PARSER_GeneratedLALRParser_H
#define MPL_PARSER_GeneratedLALRParser_H
#include "../lexer.h"
#include "../ast/parser_node.h"
#include <memory>
#include <string>
#include <iostream>
#include "../ast/ast_node.h"
#include "../ast/binary_op_node.h"
#include "../ast/number_node.h"
#include "../ast/id_node.h"
#include "../ast/ast_action.h"
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
    ::mpl::ast::ParserNodePtr build();
private:
    Lexer _lexer;
};
} // namespace parser
} // namespace mpl
#endif // MPL_PARSER_GeneratedLALRParser_H
