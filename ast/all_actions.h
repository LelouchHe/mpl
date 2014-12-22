#ifndef MPL_AST_AST_ACTION_H
#define MPL_AST_AST_ACTION_H

#include "parser_node.h"

namespace mpl {
namespace ast {

void binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void id_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void parenthesis_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void simple_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);

} // namespace ast
} // namespace mpl

#endif // MPL_AST_AST_ACTION_H