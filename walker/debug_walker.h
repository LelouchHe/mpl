#ifndef MPL_WALKER_DEBUG_WALKER_H
#define MPL_WALKER_DEBUG_WALKER_H

#include <vector>
#include "walker.h"

namespace mpl {
namespace walker {

class DebugWalker;
typedef std::shared_ptr<DebugWalker> DebugWalkerPtr;

class DebugWalker : public Walker {
public:
	virtual ~DebugWalker();

public:
	virtual void walk();

	static DebugWalkerPtr create(const ::mpl::ASTNodePtr& root);

private:
	DebugWalker(const ::mpl::ASTNodePtr& root);
};


} // namespace walker
} // namespace mpl

#endif // MPL_WALKER_DEBUG_WALKER_H