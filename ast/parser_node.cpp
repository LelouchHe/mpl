#include "parser_node.h"

#include <iostream>
#include <cassert>

#include "binary_op_node.h"
#include "number_node.h"

namespace mpl {
namespace ast {

ParserNodePtr ParserNode::create(const Token& token) {
	return ParserNodePtr(new ParserNode(token));
}

void ParserNode::binary_op_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	switch (right[1]->_token.type) {
	case Token::TokenType::TT_PLUS:
		left->_ast = BinaryOpNode::create(BinaryOpNode::PLUS, right[0]->_ast, right[2]->_ast);
		break;

	case Token::TokenType::TT_MINUS:
		left->_ast = BinaryOpNode::create(BinaryOpNode::MINUS, right[0]->_ast, right[2]->_ast);
		break;

	case Token::TokenType::TT_MUL:
		left->_ast = BinaryOpNode::create(BinaryOpNode::MUL, right[0]->_ast, right[2]->_ast);
		break;

	case Token::TokenType::TT_DIV:
		left->_ast = BinaryOpNode::create(BinaryOpNode::DIV, right[0]->_ast, right[2]->_ast);
		break;

	default:
		break;
	}
}

void ParserNode::number_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);

	left->_ast = NumberNode::create(right[0]->_token.text);
}

void ParserNode::parenthesis_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 3);

	left->_ast = right[1]->_ast;
}

void ParserNode::simple_action(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right) {
	assert(right.size() == 1);
	left->_ast = right[0]->_ast;
}

ParserNode::ParserNode(const Token& token) :
		_token(token) {

}

ParserNode::~ParserNode() {

}

void ParserNode::add(const ParserNodePtr& node) {
	_children.push_back(node);
}

size_t ParserNode::size() const {
	return _children.size();
}

const ParserNodePtr& ParserNode::operator[](size_t s) const {
	assert(s < _children.size());

	return _children[s];
}

const ParserNode::Token& ParserNode::token() const {
	return _token;
}

void ParserNode::reduce(ReduceAction action) {
	if (action != NULL) {
		action(shared_from_this(), _children);
	}
}

const ASTNodePtr& ParserNode::ast() const {
	return _ast;
}

void ParserNode::debug() const {
	std::vector<bool> is_last;
	debug(&is_last);
}

void ParserNode::debug(std::vector<bool>* pis_last) const {
	std::vector<bool>& is_last = *pis_last;

	if (!is_last.empty()) {
		for (size_t i = 0; i < is_last.size() - 1; i++) {
			if (is_last[i]) {
				std::cout << "   ";
			} else {
				std::cout << " | ";
			}
		}
		std::cout << " |-";
	}

	std::cout << "(" << _token.type << ", " << _token.text << ")" << std::endl;
	for (size_t i = 0; i < _children.size(); i++) {
		is_last.push_back(i == _children.size() - 1);
		
		_children[i]->debug(pis_last);

		is_last.pop_back();
	}
}

} // namespace ast
} // namespace mpl