#ifndef MPL_AST_AST_NODE_H
#define MPL_AST_AST_NODE_H

#include <memory>
#include <vector>
#include <string>

namespace mpl {
namespace ast {

enum ASTType {
	// ��ƽ����
	AT_LIST = 0,

	// �﷨�ɷ�
	AT_ASSIGN, AT_FUNC_DEF, AT_FUNC_CALL, AT_RETURN,

	// ����
	AT_MUL, AT_DIV, AT_PLUS, AT_MINUS,

	// terminal
	AT_NUMBER, AT_ID, AT_STRING,
};

extern const std::vector<std::string> AST_NAMES;

class ASTNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;

// ����������α��?
// number/id/string?
class ASTNode {
public:
	virtual ~ASTNode();

public:
	ASTType type() const;

	void debug() const;
	virtual void debug(std::vector<bool>* pis_last) const = 0;
	
protected:
	void debug_prefix(std::vector<bool>* pis_last) const;

	ASTNode(ASTType type);

protected:
	ASTType _type;
};

} // namespace ast
} // namespace mpl

#endif // MPL_AST_AST_NODE_H