#ifndef MPL_TYPE_SCOPE_H
#define MPL_TYPE_SCOPE_H

#include <string>

namespace mpl {
namespace type {

class Scope {
public:
	virtual ~Scope();

public:
	void define(const std::string& name);

};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_SCOPE_H