#include "bool_node.h"

namespace mpl {
namespace ast {

BoolNodePtr BoolNode::create(bool b) {
	if (b) {
		return BoolNodePtr(new BoolNode(::mpl::ASTType::AT_TRUE, "true"));
	} else {
		return BoolNodePtr(new BoolNode(::mpl::ASTType::AT_FALSE, "false"));
	}
}

BoolNode::BoolNode(::mpl::ASTType type, const std::string& text) :
		::mpl::ASTNode(type), _text(text) {
}

BoolNode::~BoolNode() {

}

const std::string& BoolNode::text() const {
	return _text;
}

} // namespace ast
} // namespace mpl