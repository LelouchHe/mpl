#include "id_node.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

namespace mpl {
namespace ast {

IDNodePtr IDNode::create(const std::string& text) {
	return IDNodePtr(new IDNode(text));
}

IDNode::IDNode(const std::string& text) :
		_text(text) {
}

IDNode::~IDNode() {

}

void IDNode::debug(std::vector<bool>* pis_last) const {
	debug_prefix(pis_last);

	std::cout << _text << std::endl;
}

} // namespace ast
} // namespace mpl