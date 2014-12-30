#include "debug_walker.h"

#include <iostream>
#include "../ast/all_nodes.h"

namespace mpl {
namespace walker {

DebugWalkerPtr DebugWalker::create(const ::mpl::ASTNodePtr& root) {
	return DebugWalkerPtr(new DebugWalker(root));
}

DebugWalker::DebugWalker(const ::mpl::ASTNodePtr& root) :
		Walker(root) {

}

DebugWalker::~DebugWalker() {

}

static void debug_prefix(const std::vector<bool>& is_last) {
	if (is_last.empty()) {
		return;
	}

	for (size_t i = 0; i < is_last.size() - 1; i++) {
		if (is_last[i]) {
			std::cout << "   ";
		} else {
			std::cout << " | ";
		}
	}
	std::cout << " |-";
}

static void debug(const ::mpl::ASTNodePtr& node, std::vector<bool>* pis_last) {
	debug_prefix(*pis_last);
	if (!node) {
		std::cout << "(null)" << std::endl;
		return;
	}

	switch (node->type()) {
	case ::mpl::ASTType::AT_NUMBER:
		std::cout << node->number() << std::endl;
		break;

	case ::mpl::ASTType::AT_ID:
		std::cout << node->text() << std::endl;
		break;

	case ::mpl::ASTType::AT_STRING:
		std::cout << node->text() << std::endl;
		break;

	default:
		std::cout << ::mpl::ast::AST_NAMES[node->type()] << std::endl;
		for (size_t i = 0; i < node->size(); i++) {
			pis_last->push_back(i == node->size() - 1);
			debug((*node)[i], pis_last);
			pis_last->pop_back();
		}
		break;
	}
}

void DebugWalker::walk() {
	std::vector<bool> is_last;
	debug(_root, &is_last);
}
	
} // namespace walker
} // namespace mpl

#if 0

#include "../file_reader.h"
#include "../string_reader.h"
#include "../parser.h"

using namespace std;

int main() {
	//::mpl::StringReader reader("a = 1 + 2 * b");
	::mpl::FileReader reader("parser.lua");

	::mpl::Parser parser(reader);
	::mpl::ast::ParserNodePtr root = parser.build();
	root->debug();

	::mpl::walker::DebugWalkerPtr debug_walker = ::mpl::walker::DebugWalker::create(root->ast);
	debug_walker->walk();

	return 0;
}

#endif