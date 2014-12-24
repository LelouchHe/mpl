#include "number_node.h"

#include <cstdlib>
#include <iostream>

namespace mpl {
namespace ast {

NumberNodePtr NumberNode::create(const std::string& text) {
	return NumberNodePtr(new NumberNode(text));
}

NumberNode::NumberNode(const std::string& text) :
		::mpl::ASTNode(::mpl::ASTType::AT_NUMBER),
		_text(text) {
	_number = std::strtod(_text.c_str(), NULL);
}

NumberNode::~NumberNode() {

}

double NumberNode::number() const {
	return _number;
}

const std::string& NumberNode::text() const {
	return _text;
}

void NumberNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	std::cout << _number << std::endl;
}

} // namespace ast
} // namespace mpl