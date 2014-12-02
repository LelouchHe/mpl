#ifndef MPL_PARSER_GRAMMA_H
#define MPL_PARSER_GRAMMA_H

#include <vector>
#include <map>
#include <set>

#include "../config.h"

namespace mpl {
namespace parser {

class GrammaOption {
public:
	GrammaOption() : left_recursion(true), left_factor(true) {

	}

	bool left_recursion;
	bool left_factor;
};

class Gramma {
public:
	typedef ::mpl::Lexer Lexer;
	
	typedef Lexer::Token Token;
	typedef Token::TokenType TokenType;
	typedef std::vector<Token> Rule;

	// 主要为了序列化建表方便
	// >= 0: terminal
	// <  0: |x| nonterminal
	typedef std::vector<int> InnerRule;
	typedef std::vector<InnerRule> InnerRules;
	typedef std::map<TokenType, size_t> Tran;

public:
	Gramma();
	~Gramma();

public:
	void add(const Token& token, const Rule& rule);
	bool build(GrammaOption option = GrammaOption());

	size_t size() const;
	const Tran& operator[](int token) const;
	const std::string& name(int token) const;
	const InnerRule& rule(int token, size_t index) const;

	int start() const;

	void debug();

	// 以下是generator需要的接口
	const std::vector<std::string>& nonterminals() const;
	const std::vector<InnerRules>& rules() const;
	const std::vector<Tran>& trans() const;

private:
	bool dedup();
	bool left_recursion();
	bool left_factor();
	bool generate_nullable();
	bool generate_first();
	bool generate_follow();
	bool generate_trans();

	void add_fake_start();

	bool left_factor_token(size_t token);

	size_t new_nonternimal(const std::string& name);

private:
	typedef std::set<TokenType> Tokens;
	
	std::vector<std::string> _nonterminals;
	std::vector<InnerRules> _rules;

	std::vector<Tran> _trans;

	std::vector<bool> _nullable;
	std::vector<std::vector<bool> > _rule_nullable;

	std::vector<Tokens> _first;
	std::vector<std::vector<Tokens> > _rule_first;

	std::vector<Tokens> _follow;

	int _start;
	static const Rule NON_RULE;
};

} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_GRAMMA_H