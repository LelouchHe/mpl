#include "multi_node.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

namespace mpl {
namespace ast {

const ::mpl::ASTNodePtr MultiNode::s_null_ast;

MultiNodePtr MultiNode::create() {
	return MultiNodePtr(new MultiNode());
}

MultiNode::MultiNode() {

}

MultiNode::~MultiNode() {

}

void MultiNode::add(const ::mpl::ASTNodePtr& node) {
	_nodes.push_back(node);
}

size_t MultiNode::size() const {
	return _nodes.size();
}

const ::mpl::ASTNodePtr& MultiNode::operator[](size_t s) const {
	if (s < _nodes.size()) {
		return _nodes[s];
	} else {
		return s_null_ast;
	}
}

void MultiNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	for (size_t i = 0; i < _nodes.size(); i++) {
		pis_last->push_back(i == _nodes.size() - 1);
		_nodes[i]->debug(pis_last);
		pis_last->pop_back();
	}
}

} // namespace ast
} // namespace mpl