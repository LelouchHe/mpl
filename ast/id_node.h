#ifndef MPL_AST_ID_NODE_H
#define MPL_AST_ID_NODE_H

#include <memory>
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
	static IDNodePtr create(const std::string& text);

	const std::string& text() const;

protected:
	virtual void debug(std::vector<bool>* pis_last) const;

private:
	IDNode(const std::string& text);

private:
	std::string _text;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_ID_NODE_H