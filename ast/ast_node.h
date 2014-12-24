#ifndef MPL_AST_AST_NODE_H
#define MPL_AST_AST_NODE_H

#include <memory>
#include <vector>

namespace mpl {
namespace ast {

enum ASTType {
	AT_BLOCK = 0, AT_ASSIGN, AT_FUNC_DEF, AT_FUNC_CALL,

	AT_MUL, AT_DIV, AT_PLUS, AT_MINUS,

	AT_NUMBER, AT_ID, AT_STRING,
};

class ASTNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;

class ASTNode {
public:
	~ASTNode();

public:
	static ASTNodePtr create(ASTType type);

	void add(const ASTNodePtr& node);

	size_t size() const;
	const ASTNodePtr& operator[](size_t s) const;
	ASTType type() const;

	void debug() const;
	virtual void debug(std::vector<bool>* pis_last) const = 0;
	
private:
	void debug_prefix(std::vector<bool>* pis_last) const;

	ASTNode(ASTType type);

private:
	ASTType _type;
	std::vector<ASTNodePtr> _nodes;

	static const ASTNodePtr s_null_ast;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_AST_NODE_H