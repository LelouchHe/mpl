#ifndef MPL_LEXER_H
#define MPL_LEXER_H

// lexer��������Ľӿ�Ҫ��
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
	// Token/TokenType�Ǳ��뱩¶��2������
	typedef Token Token;
	typedef TokenType TokenType;

	// next: ȡ��һ��token
	// lookahead: Ԥȡ��һ��token
	const Token& next();
	const Token& lookahead();
};

} // namespace concept
} // namespace mpl

#endif // MPL_LEXER_H