#include "ast_node.h"

#include <iostream>
#include <cassert>

namespace mpl {
namespace ast {

const std::vector<std::string> AST_NAMES = {
	"LIST", 
	
	"ASSIGN", "FUNC_DEF", "FUNC_CALL", "RETURN",
	"IF",

	"EXP", "MUL", "DIV", "MOD", "PLUS", "MINUS",
	"LESS", "LESS_EQUAL", "GREATER", "GREATER_EQUAL",
	"EQUAL", "NOT_EQUAL",
	"AND", "OR", "NOT",
	"CONCAT", "LEN", 
	
	"NUMBER", "ID", "STRING", "TRUE", "FALSE", "NIL",
};

ASTNode::ASTNode(ASTType type): _type(type) {

}

ASTNode::~ASTNode() {

}

ASTType ASTNode::type() const {
	return _type;
}

double ASTNode::number() const {
	assert(false);
	return 0.0;
}

static const std::string s_null_str;

const std::string& ASTNode::text() const {
	assert(false);
	return s_null_str;
}

void ASTNode::add(const ASTNodePtr& node) {
	assert(false);
}

size_t ASTNode::size() const {
	return 1;
}

ASTNodePtr ASTNode::operator[](size_t s) const {
	assert(s < 1);

	return const_cast<ASTNode *>(this)->shared_from_this();
}

} // namespace ast
} // namespace mpl