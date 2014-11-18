#ifndef MPL_REGEX_H
#define MPL_REGEX_H

#include "dfa_generator.h"

namespace mpl {

class Regex {
public:
	Regex();
	~Regex();

	bool parse(const char* pattern);

	bool full_match(const char* str);
	bool partial_match(const char* str, char** end);

private:
	std::string _pattern;
	::mpl::DFAGenerator _dfa;
};

} // namespace mpl

#endif // MPL_REGEX_H