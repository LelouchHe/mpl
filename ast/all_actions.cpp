#include "all_actions.h"

#include <cassert>
#include "all_nodes.h"

namespace mpl {
namespace ast {

typedef ::mpl::Lexer::Token Token;

void binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	ListNodePtr ast;

	switch (right[1]->token.type) {
	case Token::TokenType::TT_PLUS:
		ast = ListNode::create(::mpl::ASTType::AT_PLUS);
		break;

	case Token::TokenType::TT_MINUS:
		ast = ListNode::create(::mpl::ASTType::AT_MINUS);
		break;

	case Token::TokenType::TT_MUL:
		ast = ListNode::create(::mpl::ASTType::AT_MUL);
		break;

	case Token::TokenType::TT_DIV:
		ast = ListNode::create(::mpl::ASTType::AT_DIV);
		break;

	default:
		assert(false);
		break;
	}

	ast->add(right[0]->ast);
	ast->add(right[2]->ast);
	left->ast = ast;
}

void number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->ast = NumberNode::create(right[0]->token.text);
}

void id_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->ast = IDNode::create(right[0]->token.text);
}

void string_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->ast = StringNode::create(right[0]->token.text);
}

void parenthesis_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	left->ast = right[1]->ast;
}

void simple_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);
	left->ast = right[0]->ast;
}

void assign_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	ListNodePtr multi = ListNode::create(::mpl::ASTType::AT_ASSIGN);
	multi->add(right[0]->ast);
	multi->add(right[2]->ast);

	left->ast = multi;
}

void func_def_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	ListNodePtr multi = ListNode::create(::mpl::ASTType::AT_FUNC_DEF);
	multi->add(right[1]->ast);
	multi->add(right[2]->ast);

	left->ast = multi;
}

void func_body_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 5);

	ListNodePtr multi = ListNode::create(::mpl::ASTType::AT_LIST);
	multi->add(right[1]->ast);
	multi->add(right[3]->ast);

	left->ast = multi;
}

void func_call_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 2);

	ListNodePtr multi = ListNode::create(::mpl::ASTType::AT_FUNC_CALL);
	multi->add(right[0]->ast);
	multi->add(right[1]->ast);

	left->ast = multi;
}

void return_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 2);

	left->ast = ListNode::create(::mpl::ASTType::AT_RETURN);
}

void return_exp_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	ListNodePtr multi = ListNode::create(::mpl::ASTType::AT_RETURN);
	multi->add(right[1]->ast);

	left->ast = multi;
}

void assign(const ParserNodePtr& left, const ParserNodePtr& right) {
	left->ast = right->ast;
}

void append(const ParserNodePtr& left, const ParserNodePtr& right) {
	ListNodePtr multi = std::dynamic_pointer_cast<ListNode>(left->ast);
	assert(multi);
	multi->add(right->ast);
}

void loop(const ParserNodePtr& left, const ParserNodePtr& head, const ParserNodePtr& item) {
	if (head->ast->type() == ::mpl::ASTType::AT_LIST) {
		append(head, item);
		left->ast = head->ast;
	} else {
		ListNodePtr multi = ListNode::create(::mpl::ASTType::AT_LIST);
		multi->add(head->ast);
		multi->add(item->ast);

		left->ast = multi;
	}
}

} // namespace ast
} // namespace mpl