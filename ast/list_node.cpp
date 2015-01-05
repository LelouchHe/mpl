#include "list_node.h"

#include <cassert>

namespace mpl {
namespace ast {

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

::mpl::ASTNodePtr ListNode::operator[](size_t s) const {
	assert(s < _nodes.size());

	return _nodes[s];
}

} // namespace ast
} // namespace mpl