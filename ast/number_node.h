#ifndef MPL_AST_NUMBER_NODE_H
#define MPL_AST_NUMBER_NODE_H

#include <string>
#include "ast_node.h"

namespace mpl {
namespace ast {

class NumberNode;
typedef std::shared_ptr<NumberNode> NumberNodePtr;

class NumberNode : public ASTNode {
public:
	virtual ~NumberNode();

public:
	virtual void debug(std::vector<bool>* pis_last) const;

	static NumberNodePtr create(const std::string& text);

private:
	std::string _text;
	double _number;

private:
	NumberNode(const std::string& text);
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_NUMBER_NODE_H