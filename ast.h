#ifndef MPL_AST_H
#define MPL_AST_H

#define AST_MANUAL 1
#define AST_GENERATED 0

#include <memory>

#if AST_MANUAL
#include "ast/ast_node.h"
#elif AST_GENERATED
#include "ast/ast_node.h"
#endif

namespace mpl {

#if AST_MANUAL
typedef ::mpl::ast::ASTNode ASTNode;
#elif AST_GENERATED
typedef ::mpl::ast::ASTNode ASTNode;
#endif

typedef std::shared_ptr<ASTNode> ASTNodePtr;

} // namespace mpl

#endif // MPL_AST_H