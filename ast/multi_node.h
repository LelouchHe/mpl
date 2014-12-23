#ifndef MPL_AST_MULTI_NODE_H
#define MPL_AST_MULTI_NODE_H

#include <vector>
#include "../ast.h"

namespace mpl {
namespace ast {

class MultiNode;
typedef std::shared_ptr<MultiNode> MultiNodePtr;

class MultiNode : public ::mpl::ASTNode {
public:
	virtual ~MultiNode();

public:
	void add(const ::mpl::ASTNodePtr& node);
	size_t size() const;
	const ::mpl::ASTNodePtr& operator[](size_t s) const;

	virtual void debug(std::vector<bool>* pis_last) const;

	static MultiNodePtr create();

private:
	std::vector< ::mpl::ASTNodePtr> _nodes;

	static const ::mpl::ASTNodePtr s_null_ast;

private:
	MultiNode();
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_MULTI_NODE_H