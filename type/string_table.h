#ifndef MPL_TYPE_STRING_TABLE_H
#define MPL_TYPE_STRING_TABLE_H

#include <vector>

namespace mpl {
namespace type {

class String;

class StringTable {
public:
	StringTable();
	~StringTable();

public:
	String* add(const char* str);
	String* add(const std::string& str);

	size_t size() const;
	String* operator[](size_t s) const;

private:
	std::vector<String *> _strs;

private:
	StringTable(const StringTable&);
	StringTable& operator=(const StringTable&);
};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_STRING_TABLE_H