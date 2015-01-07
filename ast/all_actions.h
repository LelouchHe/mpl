#ifndef MPL_AST_AST_ACTION_H
#define MPL_AST_AST_ACTION_H

#include "parser_node.h"

namespace mpl {
namespace ast {

// 这些action是将ParserNode转化为ASTNode
// 和后期我们要进行的walk不一样,此时还没有ASTTree

void binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void unary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void id_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void parenthesis_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void simple_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void assign_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void func_def_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void func_body_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void func_call_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void return_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void return_exp_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void if_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void elseif_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
void else_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);

// 一些便捷操作
void assign(const ParserNodePtr& left, const ParserNodePtr& right);
void append(const ParserNodePtr& left, const ParserNodePtr& right);
void loop(const ParserNodePtr& left, const ParserNodePtr& head, const ParserNodePtr& item);

} // namespace ast
} // namespace mpl

#endif // MPL_AST_AST_ACTION_H