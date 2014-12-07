#include "grammar.h"

#include <iostream>
#include "../../string_reader.h"

namespace mpl {
namespace parser {
namespace detail {

Grammar::Grammar() : _start(0) {
	// 0专门空出
	_nonterminals.push_back("");
	_rules.push_back(InnerRules());
}

Grammar::~Grammar() {

}

size_t Grammar::new_nonternimal(const std::string& name) {
	assert(_nonterminals.size() == _rules.size());

	std::vector<std::string>::iterator it = std::find(_nonterminals.begin(), _nonterminals.end(), name);
	if (it != _nonterminals.end()) {
		return it - _nonterminals.begin();
	} else {
		size_t size = _nonterminals.size();

		_nonterminals.push_back(name);
		_rules.push_back(InnerRules());

		return size;
	}
}

// 前后追赶
static std::vector<std::string> split(const std::string& str, char delim) {
	std::vector<std::string> strs;

	size_t begin = 0;
	size_t end = begin;
	while (end < str.size()) {
		if (str[end] == delim) {
			while (begin < end && str[begin] == delim) {
				begin++;
			}
			if (begin != end) {
				strs.push_back(str.substr(begin, end - begin));
				begin = end;
			}
		}
		end++;
	}
	while (begin < end && str[begin] == delim) {
		begin++;
	}
	if (begin != end) {
		strs.push_back(str.substr(begin, end - begin));
	}

	return strs;
}

void Grammar::add(const std::string& token, const std::string& rule) {
	int left = new_nonternimal(token);
	if (_start == 0) {
		_start = index2token(left);
	}

	InnerRule inner_rule;
	std::vector<std::string> strs = split(rule, ' ');
	for (size_t j = 0; j < strs.size(); j++) {
		const std::string& str = strs[j];
		if (str.empty()) {
			continue;
		}

		Token token;
		if (str[0] == '\'') {
			assert(str[str.size() - 1] == '\'');

			::mpl::StringReader reader(str.substr(1, str.size() - 2));
			Lexer lexer(reader);
			token = lexer.next();
			assert(lexer.lookahead().type == TokenType::EOS);
		} else {
			TokenType type = Lexer::token_type(str);
			if (type != TokenType::ERROR) {
				token.type = type;
			} else {
				token.type = TokenType::NONTERMINAL;
			}
			token.text = str;
		}

		if (token.type != TokenType::NONTERMINAL) {
			inner_rule.push_back((int)token.type);
		} else {
			int index = new_nonternimal(token.text);
			inner_rule.push_back(index2token(index));
		}
	}
	_rules[left].push_back(inner_rule);
}


size_t Grammar::size() const {
	return _nonterminals.size();
}

const std::string& Grammar::name(int token) const {
	if (token >= 0) {
		return Lexer::token_name((TokenType)token);
	} else {
		token = token2index(token);
		assert(token > 0 && (size_t)token < _nonterminals.size());
		return _nonterminals[token];
	}
}

const Grammar::InnerRule& Grammar::rule(int token, size_t index) const {
	token = token2index(token);
	assert(token > 0 && (size_t)token < _nonterminals.size());

	const InnerRules& rules = _rules[token];
	assert(index < rules.size());

	return rules[index];
}

int Grammar::start() const {
	return _start;
}

const std::vector<std::string>& Grammar::nonterminals() const {
	return _nonterminals;
}

const std::vector<Grammar::InnerRules>& Grammar::rules() const {
	return _rules;
}


void Grammar::debug() const {
	std::cout << "start @ " << name(_start) << std::endl;
	for (size_t i = NONTERMINAL_START; i < _rules.size(); i++) {
		const InnerRules& rules = _rules[i];
		for (size_t j = 0; j < rules.size(); j++) {
			const InnerRule& rule = rules[j];
			std::cout << _nonterminals[i] << " : ";
			for (size_t k = 0; k < rule.size(); k++) {
				std::cout << name(rule[k]) << " ";
			}
			std::cout << std::endl;
		}
	}

	std::cout << " --nullable--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _nullable.size(); i++) {
		std::cout << _nonterminals[i] << " : " << _nullable[i] << std::endl;
	}

	std::cout << " --first--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _first.size(); i++) {
		std::cout << _nonterminals[i] << " : ";
		const Tokens& tokens = _first[i];
		for (Tokens::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
			std::cout << Lexer::token_name((TokenType)*it) << " ";
		}
		std::cout << std::endl;
	}

	std::cout << " --follow--" << std::endl;
	for (size_t i = NONTERMINAL_START; i < _follow.size(); i++) {
		std::cout << _nonterminals[i] << " : ";
		const Tokens& tokens = _follow[i];
		for (Tokens::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
			std::cout << Lexer::token_name((TokenType)*it) << " ";
		}
		std::cout << std::endl;
	}
}

void Grammar::add_fake_start(bool add_eos) {
	int new_start = new_nonternimal("*" + name(_start));
	InnerRule rule;
	rule.push_back(_start);
	if (add_eos) {
		rule.push_back(TokenType::EOS);
	}

	_rules[new_start].push_back(rule);

	_start = index2token(new_start);
}

bool Grammar::dedup() {
	size_t size = _rules.size();
	for (size_t i = NONTERMINAL_START; i < size; i++) {
		InnerRules& rules = _rules[i];
		std::set<InnerRule> s;
		for (size_t j = 0; j < rules.size(); j++) {
			s.insert(rules[j]);
		}
		rules.clear();
		rules.insert(rules.end(), s.begin(), s.end());
	}

	return true;
}


#if 0

nullable("") = true
nullable(T) = false
nullable(a b) = nullable(a) && nullable(b)
nullable(a | b) = nullable(a) || nullable(b)

#endif

bool Grammar::generate_nullable() {
	size_t size = _nonterminals.size();
	_nullable.resize(size);

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (size_t left = NONTERMINAL_START; left < size; left++) {
			if (_nullable[left]) {
				continue;
			}

			const InnerRules& rules = _rules[left];
			for (size_t i = 0; i < rules.size(); i++) {
				const InnerRule& rule = rules[i];

				bool is_nullable = true;
				for (size_t j = 0; j < rule.size() && is_nullable; j++) {
					if (rule[j] >= 0 || !_nullable[token2index(rule[j])]) {
						is_nullable = false;
					}
				}

				if (is_nullable) {
					_nullable[left] = true;
					is_changing = true;
					break;
				}
			}
		}
	}

	return true;
}


#if 0

first("") = null
first(T) = T
first(a b) = first(a)
first(a) + first(b)  nullable(a) = true
first(a | b) = first(a) + first(b)

#endif

bool Grammar::generate_first() {
	// 可以加一个bool位标,表示是否变动,作为优化
	size_t size = _nonterminals.size();
	_first.resize(size);

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (size_t left = NONTERMINAL_START; left < size; left++) {
			Tokens& tokens = _first[left];

			const InnerRules& rules = _rules[left];
			for (size_t i = 0; i < rules.size(); i++) {
				const InnerRule& rule = rules[i];
				if (rule.empty()) {
					continue;
				}

				std::pair<Tokens::iterator, bool> ret;
				if (rule[0] >= 0) {
					ret = tokens.insert((TokenType)rule[0]);
					if (ret.second) {
						is_changing = true;
					}
					continue;
				}

				bool is_nullable = true;
				for (size_t j = 0; j < rule.size() && is_nullable; j++) {
					if (rule[j] >= 0) {
						ret = tokens.insert((TokenType)rule[j]);
						if (ret.second) {
							is_changing = true;
						}
						is_nullable = false;
					} else {
						const Tokens& right_tokens = _first[token2index(rule[j])];
						for (Tokens::const_iterator it = right_tokens.begin();
							it != right_tokens.end(); ++it) {
							ret = tokens.insert(*it);
							if (ret.second) {
								is_changing = true;
							}
						}
						is_nullable = _nullable[token2index(rule[j])];
					}
				}
			}
		}
	}

	return true;
}

#if 0

a : b c
follow(b) = first(c)
	        follow(a) nullable(c) = true

#endif

bool Grammar::generate_follow() {
	size_t size = _nonterminals.size();
	_follow.resize(size);

	bool is_changing = true;
	while (is_changing) {
		is_changing = false;

		for (size_t left = NONTERMINAL_START; left < size; left++) {
			const InnerRules& rules = _rules[left];
			for (size_t i = 0; i < rules.size(); i++) {
				const InnerRule& rule = rules[i];
				if (rule.empty()) {
					continue;
				}

				std::pair<Tokens::iterator, bool> ret;
				bool is_nullable = true;
				for (int j = rule.size() - 1; j >= 0; j--) {
					if (rule[j] >= 0) {
						is_nullable = false;
						continue;
					}

					Tokens& tokens = _follow[token2index(rule[j])];

					if ((size_t)j < rule.size() - 1) {
						int next = rule[j + 1];
						if (next >= 0) {
							ret = tokens.insert((TokenType)next);
							if (ret.second) {
								is_changing = true;
							}
							is_nullable = false;
						} else {
							const Tokens& next_tokens = _first[token2index(next)];
							for (Tokens::const_iterator it = next_tokens.begin();
								it != next_tokens.end(); ++it) {
								ret = tokens.insert(*it);
							}
						}
					}

					if (is_nullable) {
						const Tokens& left_tokens = _follow[left];
						for (Tokens::const_iterator tit = left_tokens.begin();
							tit != left_tokens.end(); tit++) {
							ret = tokens.insert(*tit);
							if (ret.second) {
								is_changing = true;
							}
						}
					}
				}
			}
		}
	}

	return true;
}



} // namespace detail
} // namespace parser
} // namespace mpl

#if 0

#include <vector>
#include <string>
#include <utility>

using namespace std;

static const vector<pair<string, string> > s_rules = {
	{ "goal", "expr" },
	{ "expr", "expr '+' term" },
	{ "expr", "expr '-' term" },
	{ "expr", "term" },
	{ "term", "term '*' factor" },
	{ "term", "term '/' factor" },
	{ "term", "factor" },
	{ "factor", "'(' expr ')'" },
	{ "factor", "NUMBER" },
	{ "factor", "ID" },

	/*
	{ "T", "R" },
	{ "T", "ID T 'c'" },
	{ "R", "" },
	{ "R", "'b' R" },
	*/
};

int main() {

	::mpl::parser::detail::Grammar grammar;
	for (size_t i = 0; i < s_rules.size(); i++) {
		grammar.add(s_rules[i].first, s_rules[i].second);
	}

	return 0;
}

#endif