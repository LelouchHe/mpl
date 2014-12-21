#include "GeneratedLALRParser.h"
#include <stack>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cassert>
namespace mpl {
namespace parser {
static const std::vector<std::string> s_nonterminals = {
	"", "goal", "expr", 
};
static const std::vector<std::vector<std::pair<std::vector<int>, int> > > s_rules = {
{
},
{
	{ {-2, 54, }, 0 },
},
{
	{ {-2, 31, -2, }, 1 },
	{ {-2, 32, -2, }, 2 },
	{ {-2, 28, -2, }, 3 },
	{ {-2, 29, -2, }, 4 },
	{ {21, -2, 22, }, 5 },
	{ {50, }, 6 },
},
};
static const int ACCEPT = 0;
static const int SHIFT = 1;
static const std::vector<std::map<int, std::pair<int, int> > > s_trans = {
{
	{-2, { 1, 1 } }, {21, { 1, 2 } }, {50, { 1, 3 } }, 
},
{
	{28, { 1, 4 } }, {29, { 1, 5 } }, {31, { 1, 6 } }, {32, { 1, 7 } }, {54, { 0, 8 } }, 
},
{
	{-2, { 1, 9 } }, {21, { 1, 2 } }, {50, { 1, 3 } }, 
},
{
	{22, { -2, 5 } }, {28, { -2, 5 } }, {29, { -2, 5 } }, {31, { -2, 5 } }, {32, { -2, 5 } }, 
	{54, { -2, 5 } }, 
},
{
	{-2, { 1, 10 } }, {21, { 1, 2 } }, {50, { 1, 3 } }, 
},
{
	{-2, { 1, 11 } }, {21, { 1, 2 } }, {50, { 1, 3 } }, 
},
{
	{-2, { 1, 12 } }, {21, { 1, 2 } }, {50, { 1, 3 } }, 
},
{
	{-2, { 1, 13 } }, {21, { 1, 2 } }, {50, { 1, 3 } }, 
},
{
	{54, { -1, 0 } }, 
},
{
	{22, { 1, 14 } }, {28, { 1, 4 } }, {29, { 1, 5 } }, {31, { 1, 6 } }, {32, { 1, 7 } }, 
},
{
	{22, { -2, 2 } }, {28, { -2, 2 } }, {29, { -2, 2 } }, {31, { -2, 2 } }, {32, { -2, 2 } }, 
	{54, { -2, 2 } }, 
},
{
	{22, { -2, 3 } }, {28, { -2, 3 } }, {29, { -2, 3 } }, {31, { -2, 3 } }, {32, { -2, 3 } }, 
	{54, { -2, 3 } }, 
},
{
	{22, { -2, 0 } }, {28, { 1, 4 } }, {29, { 1, 5 } }, {31, { -2, 0 } }, {32, { -2, 0 } }, 
	{54, { -2, 0 } }, 
},
{
	{22, { -2, 1 } }, {28, { 1, 4 } }, {29, { 1, 5 } }, {31, { -2, 1 } }, {32, { -2, 1 } }, 
	{54, { -2, 1 } }, 
},
{
	{22, { -2, 4 } }, {28, { -2, 4 } }, {29, { -2, 4 } }, {31, { -2, 4 } }, {32, { -2, 4 } }, 
	{54, { -2, 4 } }, 
},
};
static void action_0(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    assert(right.size() == 1);
    left->ast = right[0]->ast;
}
static void action_1(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    binary_op_action(left, right);
}
static void action_2(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    binary_op_action(left, right);
}
static void action_3(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    binary_op_action(left, right);
}
static void action_4(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    binary_op_action(left, right);
}
static void action_5(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    assert(right.size() == 3);
    left->ast = right[1]->ast;
}
static void action_6(
        const ::mpl::ast::ParserNodePtr& left,
        const std::vector< ::mpl::ast::ParserNodePtr>& right) {
    number_action(left, right);
}
static const std::vector< ::mpl::ast::ReduceAction> s_actions = {
	action_0, action_1, action_2, action_3, action_4, 
	action_5, action_6, 
};
GeneratedLALRParser::GeneratedLALRParser(::mpl::Reader& reader) :
        _lexer(reader) {
}
GeneratedLALRParser::~GeneratedLALRParser() {
}
::mpl::ast::ParserNodePtr GeneratedLALRParser::build() {
    std::stack<size_t> st;
    std::vector< ::mpl::ast::ParserNodePtr> nodes;
    st.push(0);
    Token token(TokenType::EPSILON, "");
    while (!st.empty()) {
        size_t state = st.top();
        std::cout << "state: " << state << "\t";
        const std::map<int, std::pair<int, int> >& tran = s_trans[state];
        bool should_next = false;
        if (token.type == TokenType::EPSILON) {
            token = _lexer.lookahead();
            should_next = true;
        }
        std::cout << "token: " << token.text << "\t";
        std::map<int, std::pair<int, int> >::const_iterator it = tran.find(token.type);
        assert(it != tran.end());
        if (it->second.first < 0) {
            int left = it->second.first;
            int rule_no = it->second.second;
            token.type = (TokenType)left;
            token.text = s_nonterminals[-left];
            std::cout << "reduce: " << token.text << std::endl;
            ::mpl::ast::ParserNodePtr parent = ::mpl::ast::ParserNode::create(token);
            const std::vector<int>& rule = s_rules[-left][rule_no].first;
            size_t children_start = nodes.size() - rule.size();
            for (size_t i = 0; i < rule.size(); i++) {
                st.pop();
                parent->add(nodes[children_start + i]);
            }
            nodes.resize(children_start);
            int action = s_rules[-left][rule_no].second;
            if (action != -1) {
                parent->reduce(s_actions[action]);
            }
            nodes.push_back(parent);
        } else if (it->second.first == ACCEPT) {
            std::cout << "accept" << std::endl;
            break;
        } else {
            st.push(it->second.second);
            std::cout << "shift: " << it->second.second << std::endl;
            if (token.type > 0) {
                nodes.push_back(::mpl::ast::ParserNode::create(token));
            }
            token.type = TokenType::EPSILON;
            token.text = "";
            if (should_next) {
                _lexer.next();
            }
        }
    }
    if (nodes.size() == 1) {
        return nodes[0];
    } else {
        return ::mpl::ast::ParserNodePtr();
    }
}
} // namespace parser
} // namespace mpl
