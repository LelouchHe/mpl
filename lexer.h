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
	// Token�Ǳ��뱩¶������
	// Token::TokenTypeҲ���붨��
	// Token.type, Token.text�Ǳ������,������Ĭ�Ϲ����˫�������캯��
	typedef Token Token;

	// next: ȡ��һ��token
	// lookahead: Ԥȡ��һ��token
	const Token& next();
	const Token& lookahead();

	// �����ַ��������,end��ʾ��β
	// auto/generater��.��Ҫ�Ǽ�parse��������
	Token parse(const char* begin, char** end);
};

} // namespace concept
} // namespace mpl

#endif // MPL_LEXER_H