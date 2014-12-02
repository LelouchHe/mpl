#include "GeneratedLLParser.h"
#include <stack>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cassert>
namespace mpl {
namespace parser {
static const std::vector<std::string> s_nonterminals = {
	"", "goal", "expr", "term", "factor", 
	"expr*", "term*", "*goal", 
};
static const std::vector<std::vector<std::vector<int> > > s_rules = {
{
},
{
	{-2, },
},
{
	{-3, -5, },
},
{
	{-4, -6, },
},
{
	{21, -2, 22, },
	{50, },
	{51, },
},
{
	{31, -3, -5, },
	{32, -3, -5, },
	{},
},
{
	{28, -4, -6, },
	{29, -4, -6, },
	{},
},
{
	{-1, 54, },
},
};
static const int s_start = -7;
static const std::vector<std::map<int, int> > s_trans = {
{
	
},
{
	{21, 0}, {50, 0}, {51, 0}, 
},
{
	{21, 0}, {50, 0}, {51, 0}, 
},
{
	{21, 0}, {50, 0}, {51, 0}, 
},
{
	{21, 0}, {50, 1}, {51, 2}, 
},
{
	{22, 2}, {31, 0}, {32, 1}, {54, 2}, 
},
{
	{22, 2}, {28, 0}, {29, 1}, {31, 2}, {32, 2}, 
	{54, 2}, 
},
{
	{21, 0}, {50, 0}, {51, 0}, 
},
};
GeneratedLLParser::GeneratedLLParser(::mpl::Reader& reader) :
        _lexer(reader) {
}
GeneratedLLParser::~GeneratedLLParser() {
}
static const std::string& name(int token) {
    if (token >= 0) {
        return ::mpl::Lexer::token_name((::mpl::Lexer::TokenType)token);
    } else {
        token = -token;
        assert(token > 0 && (size_t)token < s_nonterminals.size());
        return s_nonterminals[token];
    }
}
void GeneratedLLParser::parse() {
    std::stack<int> st;
    st.push(s_start);
    Token current = _lexer.next();
    while (!st.empty() && current.type != TokenType::ERROR) {
        int token = st.top();
        st.pop();
        std::cout << "expected: " << name(token) << std::endl;        if (token >= 0) {
            if ((TokenType)token == current.type) {
                std::cout << "match: (" << current.type << ", " << current.text << ")" << std::endl;
                current = _lexer.next();
            } else {
                std::cout << "not match : (" << current.type << ", " << current.text << ")" << std::endl;
                break;
            }
        } else {
            token = -token;
            const std::map<int, int>& tran = s_trans[token];
            std::map<int, int>::const_iterator it = tran.find(current.type);
            if (it == tran.end()) {
                std::cout << "not match" << std::endl;
                break;
            }
            const std::vector<int>& rule = s_rules[token][it->second];
            if (rule.empty()) {
                std::cout << "match empty" << std::endl;
                continue;
            }
            for (std::vector<int>::const_reverse_iterator rit = rule.rbegin(); rit != rule.rend(); ++rit) {
                st.push(*rit);
            }
        }
    }
}
} // namespace parser
} // namespace mpl
