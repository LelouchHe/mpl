#ifndef MPL_TYPE_STRING_H
#define MPL_TYPE_STRING_H

#include <string>

namespace mpl {
namespace type {

class String {
public:
	String();
	explicit String(const char* str);
	explicit String(const std::string& str);
	~String();

public:
	const std::string& str() const;
	void set(const char* str);
	void set(const std::string& str);

	String& operator=(const char* str);
	String& operator=(const std::string& str);

private:
	std::string _str;
};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_STRING_H