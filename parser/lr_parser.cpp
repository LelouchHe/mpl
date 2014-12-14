#include "lr_parser.h"

namespace mpl {
namespace parser {

template <typename Grammar>
Grammar LRParser<Grammar>::s_grammar;

template <typename Grammar>
bool LRParser<Grammar>::s_init = false;

const std::vector<std::pair<std::string, std::string> > GRAMMAR_RULES = {
	{ "s", "e" },
	{ "e", "e '+' e" },
	{ "e", "e '*' e" },
	{ "e", "NUMBER" },
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
	::mpl::StringReader sr("1 * 2 + 3");

	// ::mpl::parser::LRParser<::mpl::parser::grammar::LR1Grammar> parser(sr);
	// ::mpl::parser::LRParser<::mpl::parser::grammar::LALRGrammar> parser(sr);
	::mpl::parser::LRParser<::mpl::parser::grammar::SLRGrammar> parser(sr);


	parser.parse();

	return 0;
}

#endif