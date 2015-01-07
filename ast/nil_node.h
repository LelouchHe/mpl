#ifndef MPL_AST_NIL_NODE_H
#define MPL_AST_NIL_NODE_H

#include <memory>
#include <string>
#include "../ast.h"

namespace mpl {
namespace ast {

class NilNode;
typedef std::shared_ptr<NilNode> NilNodePtr;

class NilNode : public ::mpl::ASTNode {
public:
	virtual ~NilNode();

public:
	static NilNodePtr create();

	virtual const std::string& text() const;

private:
	NilNode(const std::string& text);

private:
	std::string _text;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_NIL_NODE_H