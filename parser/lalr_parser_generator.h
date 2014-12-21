#ifndef MPL_PARSER_LALR_PARSER_GENERATOR_H
#define MPL_PARSER_LALR_PARSER_GENERATOR_H

#include <vector>
#include <map>
#include <string>
#include <utility>

#include "grammar/lalr_grammar.h"

namespace mpl {
namespace parser {

class LALRParserGenerator {
public:
	LALRParserGenerator();
	~LALRParserGenerator();

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
	bool generate_action(std::FILE* out);
	bool generate_build(std::FILE* out, const char* parser_name);

private:
	typedef std::vector<std::string> Statement;
	typedef std::pair<std::string, int> Definition;
	typedef std::vector<Definition> Definitions;
	typedef std::pair<int, ::mpl::parser::grammar::LALRGrammar::Associativity> Attribute;

	std::vector<std::string> _includes;
	std::map<std::string, Definitions> _definitions;
	std::vector<Statement> _actions;
	std::vector<std::string> _priorities;
	std::map<std::string, Attribute> _attrs;

	::mpl::parser::grammar::LALRGrammar _grammar;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_LALR_PARSER_GENERATOR_H