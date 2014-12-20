#include "number_node.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

namespace mpl {
namespace ast {

NumberNodePtr NumberNode::create(const std::string& text) {
	return NumberNodePtr(new NumberNode(text));
}

NumberNode::NumberNode(const std::string& text) :
		_text(text) {
	char* end = NULL;
	_number = strtod(_text.c_str(), &end);
	assert(end != NULL && *end == '\0');
}

NumberNode::~NumberNode() {

}

void NumberNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	std::cout << _number << std::endl;
}

} // namespace ast
} // namespace mpl