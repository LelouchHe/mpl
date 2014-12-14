#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#define LEXER_MANUAL 0
#define LEXER_AUTO 0
#define LEXER_GENERATED 1

#if LEXER_MANUAL
#include "lexer/manual_lexer.h"
#elif LEXER_AUTO
#include "lexer/auto_lexer.h"
#elif LEXER_GENERATED
#include "lexer/GeneratedLexer.h"
#endif

namespace mpl {

#if LEXER_MANUAL
typedef ::mpl::lexer::ManualLexer Lexer;
#elif LEXER_AUTO
typedef ::mpl::lexer::AutoLexer Lexer;
#elif LEXER_GENERATED
typedef ::mpl::lexer::GeneratedLexer Lexer;
#endif

} // namespace mpl

#endif // MPL_LEXER_H