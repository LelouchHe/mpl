#ifndef MPL_PARSER_GRAMMAR_LL_GRAMMAR_H
#define MPL_PARSER_GRAMMAR_LL_GRAMMAR_H

#include <vector>
#include <map>
#include <set>

#include "grammar.h"

namespace mpl {
namespace parser {
namespace grammar {

class LLGrammarOption {
public:
	LLGrammarOption() : left_recursion(true), left_factor(true) {

	}

	bool left_recursion;
	bool left_factor;
};

class LLGrammar : public Grammar {
public:
	LLGrammar();
	~LLGrammar();

public:
	typedef std::map<TokenType, size_t> Tran;

	bool build(LLGrammarOption option = LLGrammarOption());

	const Tran& operator[](int token) const;

	const std::vector<Tran>& trans() const;

	void debug() const;

private:
	bool left_recursion();
	bool left_factor();

	bool generate_rule_nullable();
	bool generate_rule_first();

	bool generate_trans();


	bool left_factor_token(size_t token);

private:
	std::vector<Tran> _trans;

	std::vector<std::vector<bool> > _rule_nullable;
	std::vector<std::vector<Tokens> > _rule_first;
};

} // namespace grammar
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_GRAMMAR_LL_GRAMMAR_H