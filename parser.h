#ifndef MPL_PARSER_H
#define MPL_PARSER_H

#define PARSER_LL 0
#define PARSER_LR0 0
#define PARSER_LR1 0
#define PARSER_SLR 0
#define PARSER_LALR 1

#if PARSER_LL
#include "parser/ll_parser.h"
#elif PARSER_LR0
#include "parser/lr0_parser.h"
#elif PARSER_LR1
#include "parser/lr_parser.h"
#include "parser/grammar/lr1_grammar.h"
#include "parser/lr1_parser.h"
#elif PARSER_SLR
#include "parser/lr_parser.h"
#include "parser/grammar/slr_grammar.h"
#include "parser/slr_parser.h"
#elif PARSER_LALR
#include "parser/lr_parser.h"
#include "parser/grammar/lalr_grammar.h"
#include "parser/lalr_parser.h"
#endif

namespace mpl {

#if PARSER_LL
typedef ::mpl::parser::LLParser Parser;
#elif PARSER_LR0
typedef ::mpl::parser::LR0Parser Parser;
#elif PARSER_LR1
//typedef ::mpl::parser::LRParser< ::mpl::parser::grammar::LR1Grammar> Parser;
typedef ::mpl::parser::LR1Parser Parser;
#elif PARSER_SLR
//typedef ::mpl::parser::LRParser< ::mpl::parser::grammar::SLRGrammar> Parser;
typedef ::mpl::parser::SLRParser Parser;
#elif PARSER_LALR
//typedef ::mpl::parser::LRParser< ::mpl::parser::grammar::LALRGrammar> Parser;
typedef ::mpl::parser::LALRParser Parser;
#endif

} // namespace mpl

#endif // MPL_PARSER_H