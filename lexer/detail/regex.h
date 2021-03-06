#ifndef MPL_LEXER_DETAIL_REGEX_H
#define MPL_LEXER_DETAIL_REGEX_H

#include "dfa_generator.h"

namespace mpl {
namespace lexer {
namespace detail {

class Regex {
public:
	Regex();
	~Regex();

	bool parse(const char* pattern);

	bool full_match(const char* str);
	bool partial_match(const char* str, char** end);

private:
	std::string _pattern;
	DFAGenerator _dfa;
};

} // namespace detail
} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_DETAIL_REGEX_H