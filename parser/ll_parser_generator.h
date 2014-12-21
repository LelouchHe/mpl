#ifndef MPL_PARSER_LLPARSER_GENERATOR_H
#define MPL_PARSER_LLPARSER_GENERATOR_H

#include <string>
#include <map>
#include "grammar/ll_grammar.h"

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
		INCLUDE = 0,
		DEFINITION,
		OPERATOR,
		LAST
	};

	bool parse(const char* parser_file);
	bool build();
	bool generate(const char* parser_name);

	bool generate_header(const char* parser_name);
	bool generate_source(const char* parser_name);

	bool generate_gramma(std::FILE* out);
	bool generate_name(std::FILE* out);
	bool generate_parse(std::FILE* out, const char* parser_name);

private:
	typedef std::vector<std::string> Definition;

	std::map<std::string, Definition> _definitions;
	std::vector<std::string> _priorities;

	::mpl::parser::grammar::LLGrammar _grammar;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_LLPARSER_GENERATOR_H