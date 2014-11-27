#ifndef MPL_LEXER_H
#define MPL_LEXER_H

// lexer必须满足的接口要求
namespace mpl {
class Reader;

namespace concept {

class Token;
class TokenType;

class Lexer {
public:
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

	// 解析字符串的类别,end表示结尾
	// auto/generater有.主要是简化parse解析过程
	Token parse(const char* begin, char** end);
};

} // namespace concept
} // namespace mpl

#endif // MPL_LEXER_H