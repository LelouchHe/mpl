#include "type_checker.h"

namespace mpl {
namespace walker {

TypeCheckerPtr TypeChecker::create(const ::mpl::ASTNodePtr& root) {
	return TypeCheckerPtr(new TypeChecker(root));
}

TypeChecker::TypeChecker(const ::mpl::ASTNodePtr& root) :
		Walker(root) {

}

TypeChecker::~TypeChecker() {

}

void TypeChecker::walk() {
}
	
} // namespace walker
} // namespace mpl