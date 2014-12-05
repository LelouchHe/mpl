#ifndef MPL_PARSER_DETAIL_GRAMMAR_H
#define MPL_PARSER_DETAIL_GRAMMAR_H

#include <vector>
#include <map>
#include <string>
#include <cassert>

#include "../../config.h"

namespace mpl {
namespace parser {
namespace detail {

class Grammar {
public:
	/*
	Grammar();
	~Grammar();
	*/

public:
	typedef ::mpl::Lexer Lexer;

	typedef Lexer::Token Token;
	typedef Token::TokenType TokenType;
	typedef std::set<TokenType> Tokens;

	// 主要为了序列化建表方便
	// >= 0: terminal
	// <  0: |x| nonterminal
	typedef std::vector<int> InnerRule;
	typedef std::vector<InnerRule> InnerRules;

	void add(const std::string& token, const std::string& rule);

	size_t size() const;
	const std::string& name(int token) const;
	const InnerRule& rule(int token, size_t index) const;

	int start() const;
	const std::vector<std::string>& nonterminals() const;
	const std::vector<InnerRules>& rules() const;

	void debug() const;

protected:
	Grammar();
	~Grammar();

	// 一些公共操作
	void add_fake_start();

	bool dedup();
	bool generate_nullable();
	bool generate_first();
	bool generate_follow();

	int token2index(int token) const {
		assert(token < 0);
		return -token;
	}
	int index2token(int index) const {
		assert(index > 0);
		return -index;
	}

	size_t new_nonternimal(const std::string& name);

protected:
	std::vector<std::string> _nonterminals;
	std::vector<InnerRules> _rules;

	int _start;

	std::vector<bool> _nullable;
	std::vector<std::vector<bool> > _rule_nullable;

	std::vector<Tokens> _first;
	std::vector<std::vector<Tokens> > _rule_first;

	std::vector<Tokens> _follow;

	// static const int可以在class初始化
	static const size_t NONTERMINAL_START = 1;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_GRAMMAR_H