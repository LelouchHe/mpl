#include "string_node.h"

#include <cassert>

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

size_t StringNode::size() const {
	return 1;
}

::mpl::ASTNodePtr StringNode::operator[](size_t s) const {
	assert(s < 1);

	return const_cast<StringNode *>(this)->shared_from_this();
}

} // namespace ast
} // namespace mpl