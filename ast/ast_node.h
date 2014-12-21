#ifndef MPL_AST_AST_NODE_H
#define MPL_AST_AST_NODE_H

#include <memory>
#include <vector>

namespace mpl {
namespace ast {

class ASTNode {
public:
	virtual ~ASTNode();

public:
	void debug() const;
	virtual void debug(std::vector<bool>* pis_last) const = 0;
	
protected:
	void debug_prefix(std::vector<bool>* pis_last) const;

	ASTNode();
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_AST_NODE_H