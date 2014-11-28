#ifndef MPL_PARSER_GRAMMA_H
#define MPL_PARSER_GRAMMA_H

#include <vector>
#include <map>
#include <set>

#include "../lexer/token.h"
#include "../lexer/GeneratedLexer.h"

namespace mpl {
namespace parser {

template <typename Token>
class Gramma {
public:
	// typedef ::mpl::lexer::Token Token;
	// typedef ::mpl::lexer::GeneratedLexer::Token Token;
	typedef typename Token::TokenType TokenType;
	typedef std::vector<Token> Rule;
	typedef std::set<Token> Tokens;

public:
	Gramma();
	~Gramma();

public:
	void add(const Token& token, const Rule& rule);
	bool build();

	const Token& start() const;
	const Rule& fetch(const Token& token, const Token& next) const;

	void debug();

private:

	typedef std::vector<Rule> Rules;

	bool dedup();
	bool left_recursion();
	bool left_factor();
	bool generate_nullable();
	bool generate_first();
	bool generate_follow();
	bool left_factor_token(const Token& token, const Rules& rules, std::string* suffix, std::map<Token, Rules>* g);

	void merge(const std::map<Token, Rules>& g, bool append);
	void add_fake_start();

private:
	std::map<Token, Rules> _gramma;
	std::map<Token, bool> _nullable;
	std::map<Token, Tokens> _first;
	std::map<Token, Tokens> _follow;

	Token _start;
	static const Rule NON_RULE;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_GRAMMA_H