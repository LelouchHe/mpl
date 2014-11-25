#ifndef MPL_LEXER_AUTO_LEXER_H
#define MPL_LEXER_AUTO_LEXER_H

#include <sstream>
#include "token.h"
#include "detail/dfa_generator.h"
#include "detail/dfa_merger.h"

namespace mpl {
class Reader;

namespace lexer {

class AutoLexer {
public:
	AutoLexer(::mpl::Reader& reader);
	~AutoLexer();

public:
	typedef Token Token;
	typedef TokenType TokenType;

	const Token& next();
	const Token& lookahead();

	Token parse(const char* beign, char** end);

private:
	void init();
	TokenType lex();

private:
	::mpl::lexer::detail::DFAGenerator _generator;
	::mpl::lexer::detail::DFAMerger _merger;

	::mpl::Reader& _reader;
	std::ostringstream _buf;
	char _current;

	Token _next;
	Token _ahead;
};

} // namespace lexer
} // namespace mpl

#endif // MPL_LEXER_AUTO_LEXER_H