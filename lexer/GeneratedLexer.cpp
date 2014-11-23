#include "GeneratedLexer.h"
#include "../reader.h"
namespace mpl {
namespace lexer {
GeneratedLexer::GeneratedLexer(::mpl::Reader& reader) : 
        _reader(reader), _current('\0') {
}
GeneratedLexer::~GeneratedLexer() {}
using ::mpl::lexer::detail::DFATran;
static const std::vector<DFATran> s_trans = {
{
	{0xa, 5}, {0xd, 6}, {0x20, 9}, {0x27, 10}, {0x2a, 4}, 
	{0x2b, 8}, {0x2d, 2}, {0x31, 7}, {0x32, 7}, {0x33, 7}, 
	{0x34, 7}, {0x35, 7}, {0x36, 7}, {0x37, 7}, {0x38, 7}, 
	{0x39, 7}, {0x3d, 1}, {0x41, 3}, {0x42, 3}, {0x43, 3}, 
	{0x44, 3}, {0x45, 3}, {0x46, 3}, {0x47, 3}, {0x48, 3}, 
	{0x49, 3}, {0x4a, 3}, {0x4b, 3}, {0x4c, 3}, {0x4d, 3}, 
	{0x4e, 3}, {0x4f, 3}, {0x50, 3}, {0x51, 3}, {0x52, 3}, 
	{0x53, 3}, {0x54, 3}, {0x55, 3}, {0x56, 3}, {0x57, 3}, 
	{0x58, 3}, {0x59, 3}, {0x5a, 3}, {0x5f, 3}, {0x61, 3}, 
	{0x62, 3}, {0x63, 3}, {0x64, 3}, {0x65, 3}, {0x66, 3}, 
	{0x67, 3}, {0x68, 3}, {0x69, 3}, {0x6a, 3}, {0x6b, 3}, 
	{0x6c, 3}, {0x6d, 3}, {0x6e, 3}, {0x6f, 3}, {0x70, 3}, 
	{0x71, 3}, {0x72, 3}, {0x73, 3}, {0x74, 3}, {0x75, 3}, 
	{0x76, 3}, {0x77, 3}, {0x78, 3}, {0x79, 3}, {0x7a, 3}, 
},
{
	
},
{
	{0x2d, 11}, 
},
{
	{0x30, 3}, {0x31, 3}, {0x32, 3}, {0x33, 3}, {0x34, 3}, 
	{0x35, 3}, {0x36, 3}, {0x37, 3}, {0x38, 3}, {0x39, 3}, 
	{0x41, 3}, {0x42, 3}, {0x43, 3}, {0x44, 3}, {0x45, 3}, 
	{0x46, 3}, {0x47, 3}, {0x48, 3}, {0x49, 3}, {0x4a, 3}, 
	{0x4b, 3}, {0x4c, 3}, {0x4d, 3}, {0x4e, 3}, {0x4f, 3}, 
	{0x50, 3}, {0x51, 3}, {0x52, 3}, {0x53, 3}, {0x54, 3}, 
	{0x55, 3}, {0x56, 3}, {0x57, 3}, {0x58, 3}, {0x59, 3}, 
	{0x5a, 3}, {0x5f, 3}, {0x61, 3}, {0x62, 3}, {0x63, 3}, 
	{0x64, 3}, {0x65, 3}, {0x66, 3}, {0x67, 3}, {0x68, 3}, 
	{0x69, 3}, {0x6a, 3}, {0x6b, 3}, {0x6c, 3}, {0x6d, 3}, 
	{0x6e, 3}, {0x6f, 3}, {0x70, 3}, {0x71, 3}, {0x72, 3}, 
	{0x73, 3}, {0x74, 3}, {0x75, 3}, {0x76, 3}, {0x77, 3}, 
	{0x78, 3}, {0x79, 3}, {0x7a, 3}, 
},
{
	
},
{
	{0xd, 12}, 
},
{
	{0xa, 12}, 
},
{
	{0x30, 7}, {0x31, 7}, {0x32, 7}, {0x33, 7}, {0x34, 7}, 
	{0x35, 7}, {0x36, 7}, {0x37, 7}, {0x38, 7}, {0x39, 7}, 
},
{
	
},
{
	{0x20, 9}, 
},
{
	{0x27, 13}, {0xff, 10}, 
},
{
	{0xa, 14}, {0xd, 14}, {0xff, 11}, 
},
{
	
},
{
	
},
{
	{0xd, 1}, 
},
};
static const std::map<size_t, int> s_tags = {
	{ 1, { 0 } },
	{ 3, { 2 } },
	{ 4, { 3 } },
	{ 5, { 4 } },
	{ 7, { 5 } },
	{ 8, { 6 } },
	{ 9, { 7 } },
	{ 11, { 1 } },
	{ 12, { 4 } },
	{ 13, { 8 } },
};
static const int s_start = 0;
using ::mpl::lexer::detail::States;
static const States s_last = {
	1, 3, 4, 5, 7, 8, 9, 11, 12, 13, 
};
typedef void (GeneratedLexer::*ActionType)(Token&);
static const std::map<TokenType, ActionType> s_actions = {
	{ TT_ID, &GeneratedLexer::ID_action },
	{ TT_NEWLINE, &GeneratedLexer::NEWLINE_action },
	{ TT_NUM, &GeneratedLexer::NUM_action },
	{ TT_SPACE, &GeneratedLexer::SPACE_action },
};
void GeneratedLexer::ID_action(Token& token) {
    id_count++;
}
void GeneratedLexer::NEWLINE_action(Token& token) {
}
void GeneratedLexer::NUM_action(Token& token) {
    num_count++;
}
void GeneratedLexer::SPACE_action(Token& token) {
    token.type = SKIP;
}
TokenType GeneratedLexer::token_type(int tag) {
    TokenType type = (TokenType)tag;
    std::map<TokenType, ActionType>::const_iterator it = s_actions.find(type);
    if (it == s_actions.end()) {
        return type;
    }
    Token token;
    token.type = type;
    token.value = _buf.str();
    (this->*(it->second))(token);
    _buf.str(token.value);
    return token.type;
}
TokenType GeneratedLexer::lex() {
    _buf.str("");
    int pre = -1;
    int cur = s_start;
    if (_current == '\0') {
        _current = _reader.next();
    }
    while (!_reader.eof()) {
        if (s_last.find(cur) != s_last.end()) {
            pre = cur;
        }
        const DFATran& tran = s_trans[cur];
        DFATran::const_iterator it = tran.find(_current);
        if (it == tran.end() || it->second == -1) {
            if (it == tran.end()) {
                it = tran.find(::mpl::lexer::detail::OTHER);
            }
            if (it == tran.end() || it->second == -1) {
                cur = -1;
                break;
            }
        }
        cur = it->second;
        _buf << _current;
        _current = _reader.next();
    }
    if (s_last.find(cur) != s_last.end()) {
        pre = cur;
    }
    if (pre == -1) {
        if (_reader.eof()) {
            return EOS;
        } else {
            return ERROR;
        }
    }
    std::map<size_t, int>::const_iterator it = s_tags.find(pre);
    assert(it != s_tags.end());
    TokenType type = token_type(it->second);
    if (type == SKIP) {
        return lex();
    } else {
        return type;
    }
}
const Token& GeneratedLexer::next() {
    if (_ahead.type != EOS) {
        _next = _ahead;
        _ahead.type = EOS;
        return _next;
    }
    _next.type = lex();
    _next.value = _buf.str();
    return _next;
}
const Token& GeneratedLexer::lookahead() {
    if (_ahead.type == EOS) {
        _ahead.type = lex();
        _ahead.value = _buf.str();
    }
    return _ahead;
}
} // namespace lexer
} // namespace mpl
