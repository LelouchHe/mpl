#ifndef MPL_AST_STRING_NODE_H
#define MPL_AST_STRING_NODE_H

#include <memory>
#include <string>
#include "../ast.h"

namespace mpl {
namespace ast {

class StringNode;
typedef std::shared_ptr<StringNode> StringNodePtr;

class StringNode : public ::mpl::ASTNode {
public:
	virtual ~StringNode();

public:
	static StringNodePtr create(const std::string& text);

	const std::string& text() const;

protected:
	virtual void debug(std::vector<bool>* pis_last) const;

private:
	StringNode(const std::string& text);

private:
	std::string _text;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_STRING_NODE_H