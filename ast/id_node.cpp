#include "id_node.h"

#include <cassert>

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

size_t IDNode::size() const {
	return 1;
}

::mpl::ASTNodePtr IDNode::operator[](size_t s) const {
	assert(s < 1);

	return const_cast<IDNode *>(this)->shared_from_this();
}

} // namespace ast
} // namespace mpl