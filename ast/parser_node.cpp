#include "parser_node.h"

#include <cassert>

namespace mpl {
namespace ast {

ParserNodePtr ParserNode::create(const Token& token) {
	return ParserNodePtr(new ParserNode(token));
}

ParserNode::ParserNode(const Token& token) :
		_token(token) {

}

ParserNode::~ParserNode() {

}

void ParserNode::add(const ParserNodePtr& node) {
	_children.push_back(node);
}

size_t ParserNode::size() const {
	return _children.size();
}

const ParserNodePtr& ParserNode::operator[](size_t s) const {
	assert(s < _children.size());

	return _children[s];
}

} // namespace ast
} // namespace mpl