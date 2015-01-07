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

	case Token::TokenType::TT_CONCAT:
		ast = ListNode::create(::mpl::ASTType::AT_CONCAT);
		break;

	case Token::TokenType::TT_EXP:
		ast = ListNode::create(::mpl::ASTType::AT_EXP);
		break;

	case Token::TokenType::TT_MOD:
		ast = ListNode::create(::mpl::ASTType::AT_MOD);
		break;

	case Token::TokenType::TT_LESS:
		ast = ListNode::create(::mpl::ASTType::AT_LESS);
		break;

	case Token::TokenType::TT_LESS_EQUAL:
		ast = ListNode::create(::mpl::ASTType::AT_LESS_EQUAL);
		break;

	case Token::TokenType::TT_GREATER:
		ast = ListNode::create(::mpl::ASTType::AT_GREATER);
		break;

	case Token::TokenType::TT_GREATER_EQUAL:
		ast = ListNode::create(::mpl::ASTType::AT_GREATER_EQUAL);
		break;

	case Token::TokenType::TT_EQUAL:
		ast = ListNode::create(::mpl::ASTType::AT_EQUAL);
		break;

	case Token::TokenType::TT_NOT_EQUAL:
		ast = ListNode::create(::mpl::ASTType::AT_NOT_EQUAL);
		break;

	case Token::TokenType::TT_AND:
		ast = ListNode::create(::mpl::ASTType::AT_AND);
		break;

	case Token::TokenType::TT_OR:
		ast = ListNode::create(::mpl::ASTType::AT_OR);
		break;

	default:
		assert(false);
		break;
	}

	ast->add(right[0]->ast);
	ast->add(right[2]->ast);
	left->ast = ast;
}

void unary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 2);

	ListNodePtr ast;

	switch (right[0]->token.type) {
	case Token::TokenType::TT_NOT:
		ast = ListNode::create(::mpl::ASTType::AT_NOT);
		ast->add(right[1]->ast);
		break;

	case Token::TokenType::TT_MINUS:
		ast = ListNode::create(::mpl::ASTType::AT_MINUS);
		ast->add(NumberNode::create("0"));
		ast->add(right[1]->ast);
		break;
	
	case Token::TokenType::TT_LEN:
		ast = ListNode::create(::mpl::ASTType::AT_LEN);
		ast->add(right[1]->ast);
		break;

	default:
		assert(false);
		break;
	}

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

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_ASSIGN);
	list->add(right[0]->ast);
	list->add(right[2]->ast);

	left->ast = list;
}

void func_def_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_FUNC_DEF);
	list->add(right[1]->ast);
	list->add((*right[2]->ast)[0]);
	list->add((*right[2]->ast)[1]);

	left->ast = list;
}

void func_body_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 5);

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_LIST);
	list->add(right[1]->ast);
	list->add(right[3]->ast);

	left->ast = list;
}

void func_call_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 2);

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_FUNC_CALL);
	list->add(right[0]->ast);
	list->add(right[1]->ast);

	left->ast = list;
}

void return_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 2);

	left->ast = ListNode::create(::mpl::ASTType::AT_RETURN);
}

void return_exp_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_RETURN);
	list->add(right[1]->ast);

	left->ast = list;
}

void if_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 7);

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_IF);
	list->add(right[1]->ast);
	list->add(right[3]->ast);

	if (right[4]->ast) {
		ASTNodePtr cur = list;
		for (size_t i = 0; i < right[4]->ast->size(); i++) {
			::mpl::ASTNodePtr ast = (*right[4]->ast)[i];
			assert(ast->type() == ::mpl::ASTType::AT_IF);

			cur->add(ast);
			cur = ast;
		}

		cur->add(right[5]->ast);
	} else {
		list->add(right[5]->ast);
	}

	left->ast = list;
}

void elseif_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 4);

	ListNodePtr list = ListNode::create(::mpl::ASTType::AT_IF);
	list->add(right[1]->ast);
	list->add(right[3]->ast);

	left->ast = list;
}

void else_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 2);

	left->ast = right[1]->ast;
}

void assign(const ParserNodePtr& left, const ParserNodePtr& right) {
	left->ast = right->ast;
}

void append(const ParserNodePtr& left, const ParserNodePtr& right) {
	left->ast->add(right->ast);
}

void loop(const ParserNodePtr& left, const ParserNodePtr& head, const ParserNodePtr& item) {
	if (!head->ast || head->ast->type() != ::mpl::ASTType::AT_LIST) {
		ListNodePtr list = ListNode::create(::mpl::ASTType::AT_LIST);
		list->add(head->ast);
		list->add(item->ast);

		left->ast = list;
	} else {
		append(head, item);
		left->ast = head->ast;
	}
}

} // namespace ast
} // namespace mpl