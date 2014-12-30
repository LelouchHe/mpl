#include "string_table.h"

#include "string.h"

namespace mpl {
namespace type {

StringTable::StringTable() {
}

StringTable::~StringTable() {
	for (size_t i = 0; i < _strs.size(); i++) {
		delete _strs[i];
	}
}

String* StringTable::add(const char* str) {
	String* s = new String(str);
	_strs.push_back(s);

	return s;
}

String* StringTable::add(const std::string& str) {
	String* s = new String(str);
	_strs.push_back(s);

	return s;
}

size_t StringTable::size() const {
	return _strs.size();
}

String* StringTable::operator[](size_t s) const {
	if (s < _strs.size()) {
		return _strs[s];
	} else {
		return NULL;
	}
}

} // namespace type
} // namespace mpl