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

} // namespace ast
} // namespace mpl