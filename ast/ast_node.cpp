#include "ast_node.h"

#include <iostream>

namespace mpl {
namespace ast {

const std::vector<std::string> AST_NAMES = {
	"LIST", 
	"ASSIGN", "FUNC_DEF", "FUNC_CALL", "RETURN",
	"MUL", "DIV", "PLUS", "MINUS",
	"NUMBER", "ID", "STRING",
};

ASTNode::ASTNode(ASTType type): _type(type) {

}

ASTNode::~ASTNode() {

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