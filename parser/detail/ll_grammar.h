#ifndef MPL_PARSER_DETAIL_LL_GRAMMAR_H
#define MPL_PARSER_DETAIL_LL_GRAMMAR_H

#include <vector>
#include <map>
#include <set>

#include "grammar.h"

namespace mpl {
namespace parser {
namespace detail {

class LLGrammaOption {
public:
	LLGrammaOption() : left_recursion(true), left_factor(true) {

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

	bool build(LLGrammaOption option = LLGrammaOption());

	const Tran& operator[](int token) const;

	const std::vector<Tran>& trans() const;

	void debug() const;

private:
	bool left_recursion();
	bool left_factor();
	bool generate_trans();

	bool left_factor_token(size_t token);

private:
	std::vector<Tran> _trans;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_LL_GRAMMAR_H