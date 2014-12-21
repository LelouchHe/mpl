#ifndef MPL_AST_ID_NODE_H
#define MPL_AST_ID_NODE_H

#include <string>
#include "../ast.h"

namespace mpl {
namespace ast {

class IDNode;
typedef std::shared_ptr<IDNode> IDNodePtr;

class IDNode : public ::mpl::ASTNode {
public:
	virtual ~IDNode();

public:
	virtual void debug(std::vector<bool>* pis_last) const;

	static IDNodePtr create(const std::string& text);

private:
	std::string _text;

private:
	IDNode(const std::string& text);
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_ID_NODE_H