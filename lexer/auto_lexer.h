#ifndef MPL_LEXER_AUTO_LEXER_H
#define MPL_LEXER_AUTO_LEXER_H

#include "../lexer.h"

#include <sstream>
#include "../token.h"
#include "detail/dfa_generator.h"
#include "detail/dfa_merger.h"

namespace mpl {
class Reader;

namespace lexer {

class AutoLexer : public Lexer {
public:
	AutoLexer(::mpl::Reader& reader);
	~AutoLexer();

public:
	virtual const ::mpl::Token& next();
	virtual const ::mpl::Token& lookahead();

private:
	void init();
	::mpl::TokenType lex();

private:
	::mpl::lexer::detail::DFAGenerator _generator;
	::mpl::lexer::detail::DFAMerger _merger;

	std::ostringstream _buf;
	char _current;

	::mpl::Token _next;
	::mpl::Token _ahead;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_AUTO_LEXER_H