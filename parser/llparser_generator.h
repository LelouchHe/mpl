#ifndef MPL_PARSER_LLPARSER_GENERATOR_H
#define MPL_PARSER_LLPARSER_GENERATOR_H

#include <string>
#include <map>
#include "gramma.h"

namespace mpl {
namespace parser {

class LLParserGenerator {
public:
	LLParserGenerator();
	~LLParserGenerator();

public:
	bool build(const char* parser_file, const char* parser_name);

private:
	enum ParserState {
		START,
		DEFINITION,
		LAST
	};

	bool parse(const char* parser_file);
	bool build();
	bool generate(const char* parser_name);

private:
	typedef std::vector<std::string> Definition;
	std::map<std::string, Definition> _definitions;
	std::vector<std::string> _priorities;

	Gramma _gramma;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_LLPARSER_GENERATOR_H