#include "parser_node.h"

#include <iostream>
#include <cassert>

namespace mpl {
namespace ast {

ParserNodePtr ParserNode::create(const Token& token) {
	return ParserNodePtr(new ParserNode(token));
}

ParserNode::ParserNode(const Token& atoken) :
		token(atoken) {
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

void ParserNode::reduce(ReduceAction action) {
	if (action != NULL) {
		action(shared_from_this(), _children);
	}
}

void ParserNode::debug() const {
	std::vector<bool> is_last;
	debug(&is_last);
}

void ParserNode::debug(std::vector<bool>* pis_last) const {
	std::vector<bool>& is_last = *pis_last;

	if (!is_last.empty()) {
		for (size_t i = 0; i < is_last.size() - 1; i++) {
			if (is_last[i]) {
				std::cout << "   ";
			} else {
				std::cout << " | ";
			}
		}
		std::cout << " |-";
	}

	std::cout << "(" << token.type << ", " << token.text << ")" << std::endl;
	for (size_t i = 0; i < _children.size(); i++) {
		is_last.push_back(i == _children.size() - 1);
		
		_children[i]->debug(pis_last);

		is_last.pop_back();
	}
}

} // namespace ast
} // namespace mpl