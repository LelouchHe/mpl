#include "value.h"

#include <cassert>

#include "string.h"
#include "function.h"

namespace mpl {
namespace type {

TValuePtr TValue::create() {
	return TValuePtr(new TValue());
}

TValuePtr TValue::create(bool b) {
	return TValuePtr(new TValue(b));
}

TValuePtr TValue::create(int n) {
	return TValuePtr(new TValue(n));
}

TValuePtr TValue::create(double n) {
	return TValuePtr(new TValue(n));
}

TValuePtr TValue::create(String* s) {
	return TValuePtr(new TValue(s));
}

TValuePtr TValue::create(const char* s) {
	return TValuePtr(new TValue(new String(s)));
}

TValuePtr TValue::create(const std::string& s) {
	return TValuePtr(new TValue(new String(s)));
}

TValuePtr TValue::create(Function* f) {
	return TValuePtr(new TValue(f));
}

TValuePtr TValue::create(Type type, Value value) {
	return TValuePtr(new TValue(type, value));
}

TValue::TValue() : _type(VT_NIL) {
	_value.v = NULL;
}

TValue::TValue(bool b) : _type(VT_BOOL) {
	_value.b = b;
}

TValue::TValue(int n) : _type(VT_NUMBER) {
	_value.n = n;
}

TValue::TValue(double n) : _type(VT_NUMBER) {
	_value.n = n;
}

TValue::TValue(String* s) : _type(VT_STRING) {
	_value.s = s;
}

TValue::TValue(Function* f) : _type(VT_FUNCTION) {
	_value.f = f;
}

TValue::TValue(Type type, Value value) :
		_type(type), _value(value) {

}

TValue::~TValue() {
	clear();
}

Type TValue::type() const {
	return _type;
}

Value TValue::value() const {
	return _value;
}

TValue::operator bool() const {
	if (_type == VT_NIL) {
		return false;
	} else if (_type == VT_BOOL) {
		return _value.b;
	}

	return true;
}

double TValue::number() const {
	assert(_type == VT_NUMBER);

	return _value.n;
}

String* TValue::string() const {
	assert(_type == VT_STRING);

	return _value.s;
}

Function* TValue::function() const {
	assert(_type == VT_FUNCTION);

	return _value.f;
}

void TValue::set() {
	clear();
}

void TValue::set(bool b) {
	clear();

	_type = VT_BOOL;
	_value.b = b;
}

void TValue::set(int n) {
	clear();

	_type = VT_NUMBER;
	_value.n = n;
}

void TValue::set(double n) {
	clear();

	_type = VT_NUMBER;
	_value.n = n;
}

void TValue::set(String* s) {
	clear();

	_type = VT_STRING;
	_value.s = s;
}

void TValue::set(const char* s) {
	clear();

	_type = VT_STRING;
	_value.s = new String(s);
}

void TValue::set(const std::string& s) {
	clear();

	_type = VT_STRING;
	_value.s = new String(s);
}

void TValue::set(Function* f) {
	clear();

	_type = VT_FUNCTION;
	_value.f = f;
}

void TValue::set(Type type, Value value) {
	_type = type;
	_value = value;
}

void TValue::clear() {
	if (_type == VT_STRING) {
		delete _value.s;
	} else if (_type == VT_FUNCTION) {
		delete _value.f;
	}

	_type = VT_NIL;
	_value.v = NULL;
}

} // namespace type
} // namespace mpl

#if 0

#include <iostream>

using namespace std;

int main() {
	::mpl::type::TValuePtr a = ::mpl::type::TValue::create("hello world");
	::mpl::type::TValuePtr b = a;

	cout << a->string()->str() << endl;
	cout << b->string()->str() << endl;

	a = ::mpl::type::TValue::create(2);
	cout << a->number() << endl;
	cout << b->string()->str() << endl;

	a = b;
	a->set("fuck");
	cout << a->string()->str() << endl;
	cout << b->string()->str() << endl;

	return 0;
}

#endif