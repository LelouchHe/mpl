#ifndef MPL_AST_PARSER_NODE_H
#define MPL_AST_PARSER_NODE_H

#include <memory>
#include <vector>
#include "../lexer.h"
#include "ast_node.h"

namespace mpl {
namespace ast {

class ParserNode;
typedef std::shared_ptr<ParserNode> ParserNodePtr;

typedef void(* ReduceAction)(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);

class ParserNode : public std::enable_shared_from_this<ParserNode> {
public:
	~ParserNode();

public:
	typedef ::mpl::Lexer::Token Token;

	static ParserNodePtr create(const Token& token);

	void add(const ParserNodePtr& node);

	size_t size() const;
	const ParserNodePtr& operator[](size_t s) const;

	// 执行相关的action,将ParserNode划归为ASTNode
	void reduce(ReduceAction action);

	void debug() const;

public:
	// 暴露出来,不可修改
	const Token token;

	ASTNodePtr ast;

private:
	void debug(std::vector<bool>* pis_last) const;

private:
	std::vector<ParserNodePtr> _children;

	ParserNode(const Token& atoken);
	ParserNode(const ParserNode&);
	ParserNode& operator=(const ParserNode&);
};
	
} // namespace ast
} // namespace mpl

#endif // MPL_AST_PARSER_NODE_H