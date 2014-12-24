#ifndef MPL_AST_NUMBER_NODE_H
#define MPL_AST_NUMBER_NODE_H

#include <memory>
#include <string>
#include "../ast.h"

namespace mpl {
namespace ast {

class NumberNode;
typedef std::shared_ptr<NumberNode> NumberNodePtr;

class NumberNode : public ::mpl::ASTNode {
public:
	virtual ~NumberNode();

public:
	static NumberNodePtr create(const std::string& text);

	double number() const;
	const std::string& text() const;

protected:
	virtual void debug(std::vector<bool>* pis_last) const;

private:
	NumberNode(const std::string& text);

private:
	double _number;
	std::string _text;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_NUMBER_NODE_H