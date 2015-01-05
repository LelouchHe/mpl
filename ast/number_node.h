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

	virtual double number() const;
	virtual const std::string& text() const;

	virtual size_t size() const;
	virtual ::mpl::ASTNodePtr operator[](size_t s) const;

private:
	NumberNode(const std::string& text);

private:
	double _number;
	std::string _text;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_NUMBER_NODE_H