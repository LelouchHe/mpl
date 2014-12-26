#include "id_node.h"

#include <cstdlib>
#include <iostream>

namespace mpl {
namespace ast {

IDNodePtr IDNode::create(const std::string& text) {
	return IDNodePtr(new IDNode(text));
}

IDNode::IDNode(const std::string& text) :
		::mpl::ASTNode(::mpl::ASTType::AT_ID),
		_text(text) {
}

IDNode::~IDNode() {

}

const std::string& IDNode::text() const {
	return _text;
}

} // namespace ast
} // namespace mpl