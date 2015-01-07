#ifndef MPL_AST_AST_NODE_H
#define MPL_AST_AST_NODE_H

#include <memory>
#include <vector>
#include <string>

namespace mpl {
namespace ast {

enum ASTType {
	// 多平级项
	AT_LIST = 0,

	// 语法成分
	AT_ASSIGN, AT_FUNC_DEF, AT_FUNC_CALL, AT_RETURN,
	AT_IF,

	// 操作
	AT_EXP, AT_MUL, AT_DIV, AT_MOD, AT_PLUS, AT_MINUS, 
	AT_LESS, AT_LESS_EQUAL, AT_GREATER, AT_GREATER_EQUAL,
	AT_EQUAL, AT_NOT_EQUAL, 
	AT_AND, AT_OR, AT_NOT,
	AT_CONCAT, AT_LEN,

	// terminal
	AT_NUMBER, AT_ID, AT_STRING, AT_TRUE, AT_FALSE, AT_NIL,
};

extern const std::vector<std::string> AST_NAMES;

class ASTNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;

// 基本类型如何表达?
// number/id/string?
class ASTNode : public std::enable_shared_from_this<ASTNode> {
public:
	virtual ~ASTNode();

public:
	ASTType type() const;

	// AT_NUMBER
	virtual double number() const;

	// terminal
	virtual const std::string& text() const;

	// AT_LIST
	virtual void add(const ASTNodePtr& node);
	virtual size_t size() const;
	// 这里传值回来
	// 主要是为了让单节点仍能返回shared_from_this来统一处理
	virtual ASTNodePtr operator[](size_t s) const;
	
protected:
	ASTNode(ASTType type);

protected:
	ASTType _type;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_AST_NODE_H