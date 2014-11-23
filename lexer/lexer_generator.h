#ifndef MPL_LEXER_LEXER_GENERATOR_H
#define MPL_LEXER_LEXER_GENERATOR_H

#include <string>
#include <vector>
#include <map>

#include "detail/dfa.h"
#include "detail/dfa_generator.h"
#include "detail/dfa_merger.h"

namespace mpl {
namespace lexer {

class LexerGenerator {
public:
	LexerGenerator();
	~LexerGenerator();

	// 从lexer_file会生成lexer_name.h/.cpp,其中的lexer名字为lexer_name
	bool build(const char* lexer_file, const char* lexer_name);

private:
	enum LexerState {
		START = 0,
		GLOBAL,
		INIT,
		DEFINITION,
		ACTION,
		LAST
	};

	bool parse(const char* lexer_file);
	bool build();
	bool generate(const char* lexer_name);
	void reset();

	bool generate_header(const char* lexer_name);
	bool generate_source(const char* lexer_name);

	bool generate_token(std::FILE* out);
	bool generate_dfa(std::FILE* out);
	bool generate_trans(std::FILE* out, const ::mpl::lexer::detail::DFA& dfa);
	bool generate_tags(std::FILE* out, const ::mpl::lexer::detail::DFA& dfa);
	bool generate_start_last(std::FILE* out, const ::mpl::lexer::detail::DFA& dfa);

	bool generate_action(std::FILE* out, const char* lexer_name);

	bool generate_interface(std::FILE* out, const char* lexer_name);
	bool generate_token_type(std::FILE* out, const char* lexer_name);
	bool generate_lex(std::FILE* out, const char* lexer_name);
	bool generate_parse(std::FILE* out, const char* lexer_name);

private:
	typedef std::vector<std::string> Statment;
	// globals/inits都是以行记录,所以输出需要加'\n'
	std::vector<std::string> _includes;
	std::vector<std::string> _globals;
	std::vector<std::string> _privates;
	Statment _inits;
	std::map<std::string, std::string> _definitions;
	std::map<std::string, Statment> _actions;

	::mpl::lexer::detail::DFAGenerator _generator;
	::mpl::lexer::detail::DFAMerger _merger;
};


} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_LEXER_GENERATOR_H