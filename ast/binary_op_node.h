#ifndef MPL_AST_BINARY_OP_NODE_H
#define MPL_AST_BINARY_OP_NODE_H

#include "ast_node.h"

namespace mpl {
namespace ast {

class BinaryOpNode;
typedef std::shared_ptr<BinaryOpNode> BinaryOpNodePtr;

class BinaryOpNode : public ASTNode {
public:
	virtual ~BinaryOpNode();

public:
	enum BinaryOpType {
		PLUS,
		MINUS,
		MUL,
		DIV,
	};

	virtual void debug(std::vector<bool>* pis_last) const;

	static BinaryOpNodePtr create(
			BinaryOpType type,
			const ASTNodePtr& left, const ASTNodePtr& right);

private:
	BinaryOpType _type;
	ASTNodePtr _left;
	ASTNodePtr _right;

private:
	BinaryOpNode(BinaryOpType type,
			const ASTNodePtr& left, const ASTNodePtr& right);
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_BINARY_OP_NODE_H