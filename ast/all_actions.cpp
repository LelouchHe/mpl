#include "all_actions.h"

#include <cassert>

namespace mpl {
namespace ast {

typedef ::mpl::Lexer::Token Token;

void binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	switch (right[1]->token.type) {
	case Token::TokenType::TT_PLUS:
		left->ast = ::mpl::ASTNode::create(::mpl::ASTType::AT_PLUS);
		left->ast->add(right[0]->ast);
		left->ast->add(right[1]->ast);
		break;

	case Token::TokenType::TT_MINUS:
		left->ast = ::mpl::ASTNode::create(::mpl::ASTType::AT_MINUS);
		left->ast->add(right[0]->ast);
		left->ast->add(right[1]->ast);
		break;

	case Token::TokenType::TT_MUL:
		left->ast = ::mpl::ASTNode::create(::mpl::ASTType::AT_MUL);
		left->ast->add(right[0]->ast);
		left->ast->add(right[1]->ast);
		break;

	case Token::TokenType::TT_DIV:
		left->ast = ::mpl::ASTNode::create(::mpl::ASTType::AT_DIV);
		left->ast->add(right[0]->ast);
		left->ast->add(right[1]->ast);
		break;

	default:
		break;
	}
}

void number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->ast = NumberNode::create(right[0]->token.text);
}

void id_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->ast = IDNode::create(right[0]->token.text);
}

void parenthesis_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	left->ast = right[1]->ast;
}

void simple_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);
	left->ast = right[0]->ast;
}

} // namespace ast
} // namespace mpl