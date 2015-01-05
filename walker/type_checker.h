#ifndef MPL_WALKER_TYPE_CHECKER_H
#define MPL_WALKER_TYPE_CHECKER_H

#include "walker.h"

namespace mpl {
namespace walker {

class TypeChecker;
typedef std::shared_ptr<TypeChecker> TypeCheckerPtr;

class TypeChecker : public Walker {
public:
	virtual ~TypeChecker();

public:
	virtual void walk();

	static TypeCheckerPtr create(const ::mpl::ASTNodePtr& root);

private:
	TypeChecker(const ::mpl::ASTNodePtr& root);
};

} // namespace walker
} // namespace mpl

#endif // MPL_WALKER_TYPE_CHECKER_H