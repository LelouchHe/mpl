#include "lr_parser.h"

#include "../ast/ast_action.h"

namespace mpl {
namespace parser {

template <typename Grammar>
Grammar LRParser<Grammar>::s_grammar;

template <typename Grammar>
bool LRParser<Grammar>::s_init = false;

extern const std::vector<GrammarRule> GRAMMAR_RULES = {
	{ "s", "e", ::mpl::ast::simple_action },
	{ "e", "e '+' e", ::mpl::ast::binary_op_action },
	{ "e", "e '-' e", ::mpl::ast::binary_op_action },
	{ "e", "e '*' e", ::mpl::ast::binary_op_action },
	{ "e", "e '/' e", ::mpl::ast::binary_op_action },
	{ "e", "'(' e ')'", ::mpl::ast::parenthesis_action },
	{ "e", "NUMBER", ::mpl::ast::number_action },
};

} // namespace parser
} // namespace mpl

#if 1

#include "../file_reader.h"
#include "../string_reader.h"
#include "grammar/lr1_grammar.h"
#include "grammar/slr_grammar.h"
#include "grammar/lalr_grammar.h"

using namespace std;

int main() {
	::mpl::StringReader sr("(1 + 2) * 3");

	// ::mpl::parser::LRParser<::mpl::parser::grammar::LR1Grammar> parser(sr);
	::mpl::parser::LRParser<::mpl::parser::grammar::LALRGrammar> parser(sr);
	// ::mpl::parser::LRParser<::mpl::parser::grammar::SLRGrammar> parser(sr);

	::mpl::ast::ParserNodePtr root = parser.build();
	root->debug();

	root->ast->debug();

	return 0;
}

#endif