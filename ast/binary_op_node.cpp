#include "binary_op_node.h"

#include <iostream>

namespace mpl {
namespace ast {

BinaryOpNodePtr BinaryOpNode::create(
		BinaryOpType type,
		const ::mpl::ASTNodePtr& left, const ::mpl::ASTNodePtr& right) {
	return BinaryOpNodePtr(new BinaryOpNode(type, left, right));
}

BinaryOpNode::BinaryOpNode(BinaryOpType type,
		const ::mpl::ASTNodePtr& left, const ::mpl::ASTNodePtr& right) :
		_type(type), _left(left), _right(right) {

}

BinaryOpNode::~BinaryOpNode() {

}

void BinaryOpNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	switch (_type) {
	case PLUS:
		std::cout << "PLUS" << std::endl;
		break;

	case MINUS:
		std::cout << "MINUS" << std::endl;
		break;

	case MUL:
		std::cout << "MUL" << std::endl;
		break;

	case DIV:
		std::cout << "DIV" << std::endl;
		break;
	}
	pis_last->push_back(false);
	_left->debug(pis_last);
	pis_last->pop_back();

	pis_last->push_back(true);
	_right->debug(pis_last);
	pis_last->pop_back();
}

} // namespace ast
} // namespace mpl