#ifndef MPL_AST_BINARY_OP_NODE_H
#define MPL_AST_BINARY_OP_NODE_H

#include "../ast.h"

namespace mpl {
namespace ast {

class BinaryOpNode;
typedef std::shared_ptr<BinaryOpNode> BinaryOpNodePtr;

class BinaryOpNode : public ::mpl::ASTNode {
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
			const ::mpl::ASTNodePtr& left,
			const ::mpl::ASTNodePtr& right);

private:
	BinaryOpType _type;
	::mpl::ASTNodePtr _left;
	::mpl::ASTNodePtr _right;

private:
	BinaryOpNode(BinaryOpType type,
			const ::mpl::ASTNodePtr& left,
			const ::mpl::ASTNodePtr& right);
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_BINARY_OP_NODE_H