#ifndef MPL_WALKER_WALKER_H
#define MPL_WALKER_WALKER_H

#include <memory>
#include "../ast.h"

namespace mpl {
namespace walker {

class Walker;
typedef std::shared_ptr<Walker> WalkerPtr;

class Walker {
public:
	virtual ~Walker();

public:
	virtual void walk() = 0;

protected:
	const ::mpl::ASTNodePtr& _root;

protected:
	Walker(const ::mpl::ASTNodePtr& root);
};

} // namespace walker
} // namespace mpl

#endif // MPL_WALKER_WALKER_H