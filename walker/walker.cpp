#include "walker.h"

namespace mpl {
namespace walker {

Walker::Walker(const ::mpl::ASTNodePtr& root) :
		_root(root) {

}

Walker::~Walker() {

}

} // namespace walker
} // namespace mpl