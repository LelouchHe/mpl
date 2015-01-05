#include "scope.h"

namespace mpl {
namespace type {

ScopePtr Scope::create() {
	return ScopePtr(new Scope());
}

Scope::Scope() {

}

Scope::~Scope() {

}

void Scope::define(const std::string& name, const TValuePtr& value) {
	_vars[name] = value;
}

static const TValuePtr s_tvalue_nil = TValue::create();

const TValuePtr& Scope::find(const std::string& name) {
	ScopePtr& cur = shared_from_this();
	while (cur) {
		std::map<std::string, TValueWeakPtr>::iterator it =
				cur->_vars.find(name);
		if (it != cur->_vars.end()) {
			TValuePtr value = it->second.lock();
			if (value) {
				return value;
			}
			cur->_vars.erase(it);
		}

		cur = cur->_parent;
	}

	return s_tvalue_nil;
}

ScopePtr Scope::enter() {
	ScopePtr next(new Scope());
	next->_parent = shared_from_this();

	return next;
}

ScopePtr Scope::leave() {
	return _parent;
}

} // namespace type
} // namespace mpl

#if 0

#include <iostream>

#include "string.h"
#include "function.h"

using namespace std;

int main() {
	::mpl::type::ScopePtr cur = ::mpl::type::Scope::create();

	::mpl::type::TValuePtr a = ::mpl::type::TValue::create("hello");
	cur->define("a", a);

	cur = cur->enter();

	::mpl::type::TValuePtr t = cur->find("a");
	cout << t->string()->str() << endl;

	::mpl::type::TValuePtr c = ::mpl::type::TValue::create("world");
	cur->define("c", c);

	t = cur->find("c");
	cout << t->string()->str() << endl;

	cur = cur->leave();
	t = cur->find("c");
	cout << t->type() << endl;

	return 0;
}

#endif