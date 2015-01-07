#ifndef MPL_AST_BOOL_NODE_H
#define MPL_AST_BOOL_NODE_H

#include <memory>
#include <string>
#include "../ast.h"

namespace mpl {
namespace ast {

class BoolNode;
typedef std::shared_ptr<BoolNode> BoolNodePtr;

class BoolNode : public ::mpl::ASTNode {
public:
	virtual ~BoolNode();

public:
	static BoolNodePtr create(bool b);

	virtual const std::string& text() const;

private:
	BoolNode(::mpl::ASTType type, const std::string& text);

private:
	std::string _text;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_BOOL_NODE_H