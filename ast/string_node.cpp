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

void StringNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	std::cout << _text << std::endl;
}

} // namespace ast
} // namespace mpl