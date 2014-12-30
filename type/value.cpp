#include "value.h"

#include <cassert>

namespace mpl {
namespace type {

TValue::TValue() : _type(VT_NIL) {
	_value.p = NULL;
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
	_type = VT_NIL;
	_value.p = 0;
}

void TValue::set(bool b) {
	_type = VT_BOOL;
	_value.b = b;
}

void TValue::set(int n) {
	_type = VT_NUMBER;
	_value.n = n;
}

void TValue::set(double n) {
	_type = VT_NUMBER;
	_value.n = n;
}

void TValue::set(String* s) {
	_type = VT_STRING;
	_value.s = s;
}

void TValue::set(Function* f) {
	_type = VT_FUNCTION;
	_value.f = f;
}

void TValue::set(Type type, Value value) {
	_type = type;
	_value = value;
}

TValue& TValue::operator=(bool b) {
	set(b);
	return *this;
}

TValue& TValue::operator=(int n) {
	set(n);
	return *this;
}

TValue& TValue::operator=(double n) {
	set(n);
	return *this;
}

TValue& TValue::operator=(String* s) {
	set(s);
	return *this;
}

TValue& TValue::operator=(Function* f) {
	set(f);
	return *this;
}

} // namespace type
} // namespace mpl

#if 1

#include <iostream>

#include "string.h"
#include "string_table.h"

using namespace std;

int main() {
	::mpl::type::StringTable strs;
	::mpl::type::TValue a = strs.add("hello world");
	::mpl::type::TValue b = a;

	cout << a.string()->str() << endl;
	cout << b.string()->str() << endl;

	a = 2;
	cout << a.number() << endl;
	cout << b.string()->str() << endl;

	a = strs.add("fuck");
	b = strs.add("fuck");
	cout << a.string()->str() << endl;
	cout << b.string()->str() << endl;

	return 0;
}

#endif