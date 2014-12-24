#include "ast_node.h"

#include <iostream>

namespace mpl {
namespace ast {

const ASTNodePtr ASTNode::s_null_ast;

ASTNodePtr ASTNode::create(ASTType type) {
	return ASTNodePtr(new ASTNode(type));
}

ASTNode::ASTNode(ASTType type): _type(type) {

}

ASTNode::~ASTNode() {

}

void ASTNode::add(const ASTNodePtr& node) {
	_nodes.push_back(node);
}

size_t ASTNode::size() const {
	return _nodes.size();
}

const ASTNodePtr& ASTNode::operator[](size_t s) const {
	if (s < _nodes.size()) {
		return _nodes[s];
	} else {
		return s_null_ast;
	}
}

ASTType ASTNode::type() const {
	return _type;
}

void ASTNode::debug() const {
	std::vector<bool> is_last;
	debug(&is_last);
}

void ASTNode::debug_prefix(std::vector<bool>* pis_last) const {
	if (pis_last == NULL || pis_last->empty()) {
		return;
	}

	std::vector<bool>& is_last = *pis_last;

	for (size_t i = 0; i < is_last.size() - 1; i++) {
		if (is_last[i]) {
			std::cout << "   ";
		} else {
			std::cout << " | ";
		}
	}
	std::cout << " |-";
}

} // namespace ast
} // namespace mpl