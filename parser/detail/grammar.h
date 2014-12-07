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

	// ��ҪΪ�����л�������
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

	// һЩ��������
	// add_eosΪ�˼�ĳЩ����,����trick,�ع�ʱ����ȥ��
	void add_fake_start(bool add_eos = true);

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
	
	// nullable���Ե����ó�,Ҳ������Ϊfirst��һ����
	// ���߿��Խ�Լ����ʱ��,����������Էŵ�����Ż�ʱ����
	std::vector<bool> _nullable;
	std::vector<Tokens> _first;
	std::vector<Tokens> _follow;

	// static const int������class��ʼ��
	static const size_t NONTERMINAL_START = 1;
};

} // namespace detail
} // namespace parser
} // namespace mpl

#endif // MPL_PARSER_DETAIL_GRAMMAR_H