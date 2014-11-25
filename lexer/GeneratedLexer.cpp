#include "GeneratedLexer.h"
#include "../reader.h"
namespace mpl {
namespace lexer {
GeneratedLexer::GeneratedLexer(::mpl::Reader& reader) : 
        _reader(reader), _current('\0') {
    num_count = 0;
    id_count = 0;
}
GeneratedLexer::~GeneratedLexer() {}
using ::mpl::lexer::detail::DFATran;
static const std::vector<DFATran> s_trans = {
{
	{0xa, 7}, {0xd, 8}, {0x20, 12}, {0x27, 13}, {0x28, 5}, 
	{0x29, 11}, {0x2a, 6}, {0x2b, 10}, {0x2d, 2}, {0x2f, 3}, 
	{0x31, 9}, {0x32, 9}, {0x33, 9}, {0x34, 9}, {0x35, 9}, 
	{0x36, 9}, {0x37, 9}, {0x38, 9}, {0x39, 9}, {0x3d, 1}, 
	{0x41, 4}, {0x42, 4}, {0x43, 4}, {0x44, 4}, {0x45, 4}, 
	{0x46, 4}, {0x47, 4}, {0x48, 4}, {0x49, 4}, {0x4a, 4}, 
	{0x4b, 4}, {0x4c, 4}, {0x4d, 4}, {0x4e, 4}, {0x4f, 4}, 
	{0x50, 4}, {0x51, 4}, {0x52, 4}, {0x53, 4}, {0x54, 4}, 
	{0x55, 4}, {0x56, 4}, {0x57, 4}, {0x58, 4}, {0x59, 4}, 
	{0x5a, 4}, {0x5f, 4}, {0x61, 4}, {0x62, 4}, {0x63, 4}, 
	{0x64, 4}, {0x65, 4}, {0x66, 4}, {0x67, 4}, {0x68, 4}, 
	{0x69, 4}, {0x6a, 4}, {0x6b, 4}, {0x6c, 4}, {0x6d, 4}, 
	{0x6e, 4}, {0x6f, 4}, {0x70, 4}, {0x71, 4}, {0x72, 4}, 
	{0x73, 4}, {0x74, 4}, {0x75, 4}, {0x76, 4}, {0x77, 4}, 
	{0x78, 4}, {0x79, 4}, {0x7a, 4}, 
},
{
	
},
{
	{0x2d, 14}, 
},
{
	
},
{
	{0x30, 4}, {0x31, 4}, {0x32, 4}, {0x33, 4}, {0x34, 4}, 
	{0x35, 4}, {0x36, 4}, {0x37, 4}, {0x38, 4}, {0x39, 4}, 
	{0x41, 4}, {0x42, 4}, {0x43, 4}, {0x44, 4}, {0x45, 4}, 
	{0x46, 4}, {0x47, 4}, {0x48, 4}, {0x49, 4}, {0x4a, 4}, 
	{0x4b, 4}, {0x4c, 4}, {0x4d, 4}, {0x4e, 4}, {0x4f, 4}, 
	{0x50, 4}, {0x51, 4}, {0x52, 4}, {0x53, 4}, {0x54, 4}, 
	{0x55, 4}, {0x56, 4}, {0x57, 4}, {0x58, 4}, {0x59, 4}, 
	{0x5a, 4}, {0x5f, 4}, {0x61, 4}, {0x62, 4}, {0x63, 4}, 
	{0x64, 4}, {0x65, 4}, {0x66, 4}, {0x67, 4}, {0x68, 4}, 
	{0x69, 4}, {0x6a, 4}, {0x6b, 4}, {0x6c, 4}, {0x6d, 4}, 
	{0x6e, 4}, {0x6f, 4}, {0x70, 4}, {0x71, 4}, {0x72, 4}, 
	{0x73, 4}, {0x74, 4}, {0x75, 4}, {0x76, 4}, {0x77, 4}, 
	{0x78, 4}, {0x79, 4}, {0x7a, 4}, 
},
{
	
},
{
	
},
{
	{0xd, 15}, 
},
{
	{0xa, 15}, 
},
{
	{0x30, 9}, {0x31, 9}, {0x32, 9}, {0x33, 9}, {0x34, 9}, 
	{0x35, 9}, {0x36, 9}, {0x37, 9}, {0x38, 9}, {0x39, 9}, 
},
{
	
},
{
	
},
{
	{0x20, 12}, 
},
{
	{0x27, 16}, {0xff, 13}, 
},
{
	{0xa, 17}, {0xd, 17}, {0xff, 14}, 
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
	{ 1, 0 },
	{ 2, 5 },
	{ 3, 2 },
	{ 4, 3 },
	{ 5, 4 },
	{ 6, 6 },
	{ 7, 7 },
	{ 9, 8 },
	{ 10, 9 },
	{ 11, 10 },
	{ 12, 11 },
	{ 14, 1 },
	{ 15, 7 },
	{ 16, 12 },
};
static const int s_start = 0;
using ::mpl::lexer::detail::States;
static const States s_last = {
	1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 14, 15, 16, 
};
typedef void (GeneratedLexer::*ActionType)(GeneratedLexer::Token&);
static const std::map<GeneratedLexer::TokenType, ActionType> s_actions = {
	{ GeneratedLexer::TT_ID, &GeneratedLexer::ID_action },
	{ GeneratedLexer::TT_NEWLINE, &GeneratedLexer::NEWLINE_action },
	{ GeneratedLexer::TT_NUMBER, &GeneratedLexer::NUMBER_action },
	{ GeneratedLexer::TT_SPACE, &GeneratedLexer::SPACE_action },
};
void GeneratedLexer::ID_action(GeneratedLexer::Token& token) {
	id_count++; 
}
void GeneratedLexer::NEWLINE_action(GeneratedLexer::Token& token) {
}
void GeneratedLexer::NUMBER_action(GeneratedLexer::Token& token) {
    num_count++;
    if (num_count == 10) {
        num_count *= 2;
    }
}
void GeneratedLexer::SPACE_action(GeneratedLexer::Token& token) {
    token.type = SKIP;
}
GeneratedLexer::TokenType GeneratedLexer::token_type(int tag) {
    TokenType type = (TokenType)tag;
    std::map<TokenType, ActionType>::const_iterator it = s_actions.find(type);
    if (it == s_actions.end()) {
        return type;
    }
    Token token;
    token.type = type;
    token.text = _buf.str();
    (this->*(it->second))(token);
    _buf.str(token.text);
    return token.type;
}
bool GeneratedLexer::parse() {
    Token token;
    while (true) {
        token = next();
        if (token.type == ERROR) {
            return false;
        } else if (token.type == EOS) {
            break;
        }
    }
    return true;
}
GeneratedLexer::TokenType GeneratedLexer::lex() {
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
const GeneratedLexer::Token& GeneratedLexer::next() {
    if (_ahead.type != EOS) {
        _next = _ahead;
        _ahead.type = EOS;
        return _next;
    }
    _next.type = lex();
    _next.text = _buf.str();
    return _next;
}
const GeneratedLexer::Token& GeneratedLexer::lookahead() {
    if (_ahead.type == EOS) {
        _ahead.type = lex();
        _ahead.text = _buf.str();
    }
    return _ahead;
}
} // namespace lexer
} // namespace mpl
