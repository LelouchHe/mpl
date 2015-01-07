#include "nil_node.h"

namespace mpl {
namespace ast {

NilNodePtr NilNode::create() {
	return NilNodePtr(new NilNode("nil"));
}

NilNode::NilNode(const std::string& text) :
		::mpl::ASTNode(::mpl::ASTType::AT_NIL),
		_text(text) {
}

NilNode::~NilNode() {

}

const std::string& NilNode::text() const {
	return _text;
}

} // namespace ast
} // namespace mpl