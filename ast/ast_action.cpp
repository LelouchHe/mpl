#include "ast_action.h"

#include <cassert>

#include "binary_op_node.h"
#include "number_node.h"

namespace mpl {
namespace ast {

typedef ::mpl::Lexer::Token Token;

void binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	switch (right[1]->token.type) {
	case Token::TokenType::TT_PLUS:
		left->ast = BinaryOpNode::create(BinaryOpNode::PLUS, right[0]->ast, right[2]->ast);
		break;

	case Token::TokenType::TT_MINUS:
		left->ast = BinaryOpNode::create(BinaryOpNode::MINUS, right[0]->ast, right[2]->ast);
		break;

	case Token::TokenType::TT_MUL:
		left->ast = BinaryOpNode::create(BinaryOpNode::MUL, right[0]->ast, right[2]->ast);
		break;

	case Token::TokenType::TT_DIV:
		left->ast = BinaryOpNode::create(BinaryOpNode::DIV, right[0]->ast, right[2]->ast);
		break;

	default:
		break;
	}
}

void number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->ast = NumberNode::create(right[0]->token.text);
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