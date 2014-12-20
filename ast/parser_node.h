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
	static void binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
	static void number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
	static void parenthesis_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
	static void simple_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);

	void add(const ParserNodePtr& node);

	size_t size() const;
	const ParserNodePtr& operator[](size_t s) const;
	const Token& token() const;

	// 执行相关的action,将ParserNode划归为ASTNode
	void reduce(ReduceAction action);
	const ASTNodePtr& ast() const;

	void debug() const;

private:
	void debug(std::vector<bool>* pis_last) const;

private:
	Token _token;
	std::vector<ParserNodePtr> _children;

	ASTNodePtr _ast;

	ParserNode(const Token& token);
	ParserNode(const ParserNode&);
	ParserNode& operator=(const ParserNode&);
};
	
} // namespace ast
} // namespace mpl

#endif // MPL_AST_PARSER_NODE_H