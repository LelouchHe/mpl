#ifndef MPL_TYPE_SCOPE_H
#define MPL_TYPE_SCOPE_H

#include <memory>
#include <map>
#include <string>
#include "value.h"

namespace mpl {
namespace type {

class Scope;
typedef std::shared_ptr<Scope> ScopePtr;

class Scope : public std::enable_shared_from_this<Scope> {
public:
	~Scope();

public:
	static ScopePtr create();

	void define(const std::string& name, const TValuePtr& value);
	TValuePtr find(const std::string& name);

	ScopePtr enter();
	ScopePtr leave();

private:
	ScopePtr _parent;
	std::map<std::string, TValueWeakPtr> _vars;

	Scope();
};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_SCOPE_H