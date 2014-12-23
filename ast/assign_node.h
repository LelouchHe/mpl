#ifndef MPL_AST_ASSIGN_NODE_H
#define MPL_AST_ASSIGN_NODE_H

#include <vector>
#include "../ast.h"

namespace mpl {
namespace ast {

class AssignNode;
typedef std::shared_ptr<AssignNode> AssignNodePtr;

class AssignNode : public ::mpl::ASTNode {
public:
	virtual ~AssignNode();

public:
	void add(const ::mpl::ASTNodePtr& node);
	size_t size() const;
	const ::mpl::ASTNodePtr& operator[](size_t s) const;

	virtual void debug(std::vector<bool>* pis_last) const;

	static AssignNodePtr create();

private:
	std::vector< ::mpl::ASTNodePtr> _vars;
	std::vector< ::mpl::ASTNodePtr> _exps;

private:
	AssignNode();
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_ASSIGN_NODE_H