#ifndef MPL_LEXER_H
#define MPL_LEXER_H

#include <string>

// lexer��������Ľӿ�Ҫ��
namespace mpl {
class Reader;

namespace concept {

class Token;
class TokenType;

class Lexer {
public:
	// �ܾ���reader�ŵ����캯���ﲻ̫ǡ��,��Ϊ��2��,һ������������,��һ���Ǹ�parser���������ַ���
	// ���ǲ��������ṩ,�ͻ���һ��shared_ptr,�ѵ�ÿ�ζ�Ҫ��һ��ô?
	// ����manual_lexer�߼����ڴ�����,��γ��������?
	// ���֮����ʵ��������������,��ʵlex�Ĺ����Ǽȶ���,��������͵�����lexer�޹�
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

	// name��lexer�е�NAME
	// û��ֵ��,һ�ɱ�ERROR
	TokenType token_type(const std::string& name);
};

} // namespace concept
} // namespace mpl

#endif // MPL_LEXER_H