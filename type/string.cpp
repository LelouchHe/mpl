#include "string.h"

namespace mpl {
namespace type {

String::String() {

}

String::String(const char* str) :
		_str(str != NULL ? str : "") {

}

String::String(const std::string& str) : _str(str) {

}

String::~String() {

}

const std::string& String::str() const {
	return _str;
}

void String::set(const char* str) {
	if (str == NULL) {
		str = "";
	}
	_str.assign(str);
}

void String::set(const std::string& str) {
	_str = str;
}

String& String::operator=(const char* str) {
	set(str);
	return *this;
}

String& String::operator=(const std::string& str) {
	set(str);
	return *this;
}

} // namespace type
} // namespace mpl