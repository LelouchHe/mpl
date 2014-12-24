#include "list_node.h"

#include <iostream>

namespace mpl {
namespace ast {

const ::mpl::ASTNodePtr ListNode::s_null_node;

ListNodePtr ListNode::create(::mpl::ASTType type) {
	return ListNodePtr(new ListNode(type));
}

ListNode::ListNode(::mpl::ASTType type):
		::mpl::ASTNode(type) {

}

ListNode::~ListNode() {

}

void ListNode::add(const ::mpl::ASTNodePtr& node) {
	_nodes.push_back(node);
}

size_t ListNode::size() const {
	return _nodes.size();
}

const ::mpl::ASTNodePtr& ListNode::operator[](size_t s) const {
	if (s < _nodes.size()) {
		return _nodes[s];
	} else {
		return s_null_node;
	}
}

void ListNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	std::cout << AST_NAMES[_type] << std::endl;

	for (size_t i = 0; i < _nodes.size(); i++) {
		pis_last->push_back(i == _nodes.size() - 1);
		if (_nodes[i]) {
			_nodes[i]->debug(pis_last);
		}
		pis_last->pop_back();
	}
}

} // namespace ast
} // namespace mpl