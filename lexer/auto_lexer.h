#ifndef MPL_LEXER_AUTO_LEXER_H
#define MPL_LEXER_AUTO_LEXER_H

#include "../lexer.h"
#include "../token.h"
#include "detail/dfa_generator.h"

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

private:
	::mpl::Reader& _reader;
	::mpl::lexer::detail::DFAGenerator _dfa;

	char _current;

	::mpl::Token _next;
	::mpl::Token _ahead;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_AUTO_LEXER_H