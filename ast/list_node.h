#ifndef MPL_AST_LIST_NODE_H
#define MPL_AST_LIST_NODE_H

#include <memory>
#include <vector>
#include "../ast.h"

namespace mpl {
namespace ast {

class ListNode;
typedef std::shared_ptr<ListNode> ListNodePtr;

class ListNode : public ::mpl::ASTNode {
public:
	virtual ~ListNode();

public:
	static ListNodePtr create(::mpl::ASTType type);

	virtual void add(const ::mpl::ASTNodePtr& node);

	virtual size_t size() const;
	virtual const ::mpl::ASTNodePtr& operator[](size_t s) const;

private:
	ListNode(::mpl::ASTType type);

private:
	std::vector< ::mpl::ASTNodePtr> _nodes;

	static const ::mpl::ASTNodePtr s_null_node;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_LIST_NODE_H