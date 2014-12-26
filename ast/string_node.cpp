#include "string_node.h"

#include <cstdlib>
#include <iostream>

namespace mpl {
namespace ast {

StringNodePtr StringNode::create(const std::string& text) {
	return StringNodePtr(new StringNode(text));
}

StringNode::StringNode(const std::string& text) :
		::mpl::ASTNode(::mpl::ASTType::AT_STRING),
		_text(text) {
}

StringNode::~StringNode() {

}

const std::string& StringNode::text() const {
	return _text;
}

} // namespace ast
} // namespace mpl