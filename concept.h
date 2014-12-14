#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#include <string>

// lexer必须满足的接口要求
namespace mpl {
class Reader;

namespace concept {

class Token;
class TokenType;

class Lexer {
public:
	// 总觉得reader放到构造函数里不太恰当,因为有2套,一套是真正解析,另一个是给parser用来解析字符串
	// 但是不在这里提供,就会变成一个shared_ptr,难道每次都要来一个么?
	// 而且manual_lexer逻辑绑定在代码里,如何抽象出来呢?
	// 解决之道其实就是轻量化操作,其实lex的规则都是既定的,所以这个和单个的lexer无关
	Lexer(::mpl::Reader& reader);
	~Lexer();

public:
	// Token是必须暴露的类型
	// Token::TokenType也必须定义
	// Token.type, Token.text是必须参数,必须有默认构造和双参数构造函数
	typedef Token Token;

	// next: 取下一个token
	// lookahead: 预取下一个token
	const Token& next();
	const Token& lookahead();

	// name是lexer中的NAME
	// 没有值的,一律标ERROR
	TokenType token_type(const std::string& name);
};

} // namespace concept
} // namespace mpl

#endif // MPL_LEXER_H