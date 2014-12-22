#include "lr_parser.h"

#include "../ast/all_actions.h"

namespace mpl {
namespace parser {

template <typename Grammar>
Grammar LRParser<Grammar>::s_grammar;

template <typename Grammar>
bool LRParser<Grammar>::s_init = false;

extern const std::vector<GrammarRule> GRAMMAR_RULES = {
	{ "chunk", "block", -1 },
	{ "block", "block_0 retstat", -1 },
	{ "block_0", "stat", -1 },
	{ "block_0", "stat block_0", -1 },
	{ "stat", "';'", -1 },
	{ "retstat", "'return' ';'", -1 },
	
	/*
	{ "s", "e", 0 },
	{ "e", "e '+' e", 1 },
	{ "e", "e '-' e", 1 },
	{ "e", "e '*' e", 1 },
	{ "e", "e '/' e", 1 },
	{ "e", "'(' e ')'", 2 },
	{ "e", "NUMBER", 3 },
	{ "e", "ID", 4 },
	*/
};

extern const std::vector< ::mpl::ast::ReduceAction> ACTIONS = {
	::mpl::ast::simple_action,
	::mpl::ast::binary_op_action,
	::mpl::ast::parenthesis_action,
	::mpl::ast::number_action,
	::mpl::ast::id_action,
};

} // namespace parser
} // namespace mpl

#if 1

#include "../file_reader.h"
#include "../string_reader.h"
#include "grammar/lr1_grammar.h"
#include "grammar/slr_grammar.h"
#include "grammar/lalr_grammar.h"
#include "GeneratedLALRParser.h"
#include "../parser.h"

using namespace std;

int main() {
	//::mpl::StringReader reader("a = 1 + 2 * b");
	::mpl::FileReader reader("parser.lua");

	// ::mpl::parser::LRParser<::mpl::parser::grammar::LR1Grammar> parser(reader);
	//::mpl::parser::LRParser<::mpl::parser::grammar::LALRGrammar> parser(reader);
	// ::mpl::parser::LRParser<::mpl::parser::grammar::SLRGrammar> parser(reader);
	// ::mpl::parser::GeneratedLALRParser parser(reader);

	::mpl::Parser parser(reader);
	::mpl::ast::ParserNodePtr root = parser.build();
	root->debug();

	//root->ast->debug();

	return 0;
}

#endif