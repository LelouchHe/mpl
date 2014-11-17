#include "lexer.h"

#include <cctype>
#include <cassert>
#include "reader.h"

/*
TT_AND, TT_BREAK, TT_DO, TT_ELSE, TT_ELSEIF, TT_END,
TT_FALSE, TT_FOR, TT_FUNCTION, TT_GOTO, TT_IF, TT_IN,
TT_LOCAL, TT_NIL, TT_NOT, TT_OR, TT_REPEAT, TT_RETURN,
TT_THEN, TT_TRUE, TT_UNTIL, TT_WHILE,
*/

namespace mpl {

std::map<std::string, TokenType> Lexer::_s_reserved_tokens = {
	{ "and", TT_AND }, { "break", TT_BREAK }, { "do", TT_DO },
	{ "else", TT_ELSE }, { "end", TT_END }, { "false", TT_FALSE },
	{ "for", TT_FOR }, { "function", TT_FUNCTION }, { "goto", TT_GOTO },
	{ "if", TT_IF }, { "in", TT_IN }, { "local", TT_LOCAL },
	{ "nil", TT_NIL }, { "not", TT_NOT }, { "or", TT_OR },
	{ "repeat", TT_REPEAT }, { "return", TT_RETURN },
	{ "then", TT_THEN }, { "true", TT_TRUE }, { "until", TT_UNTIL },
	{ "while", TT_WHILE }
};

Lexer::Lexer(::mpl::Reader& reader)
	:_reader(reader), _line_num(0) {
}

Lexer::~Lexer() {

}

Token Lexer::next() {
	if (_ahead.type != TT_EOS) {
		_next = _ahead;
		_ahead.type = TT_EOS;
		return _next;
	}

	_next.type = lex();
	_next.text = _buff.str();

	return _next;
}

// ����lookahead,������ֵͬ
Token Lexer::lookahead() {
	if (_ahead.type == TT_EOS) {
		_ahead.type = lex();
		_ahead.text = _buff.str();
	}

	return _ahead;
}

void Lexer::new_line(bool should_save) {
	char ch = _reader.next();
	if (_current == '\r') {
		if (ch == '\n') {
			// "\r\n" -> windows
			if (should_save) {
				_buff << "\r\n";
			}
			_current = _reader.next();
		} else {
			// "\r" -> Mac
			if (should_save) {
				_buff << '\r';
			}
			_current = ch;
		}
	} else {
		// "\n" -> linux
		if (should_save) {
			_buff << '\n';
		}
		_current = ch;
	}

	_line_num++;
}

// ���浱ǰֵ,��ȡ��һ��ֵ
void Lexer::save_and_next() {
	_buff << _current;
	_current = _reader.next();
}

void Lexer::nosave_and_next() {
	_current = _reader.next();
}

int Lexer::count_nosave(char ch) {
	int n = 0;
	while (_current == 'ch') {
		n++;
		nosave_and_next();
	}

	return n;
}

// lex���õ����ַ�����_buff,����������,������_next/_ahead
// _current������һ���ַ�,����'\0',��ʾ��û�ж�ȡ
TokenType Lexer::lex() {
	_buff.str("");

	if (_current == '\0') {
		_current = _reader.next();
	}

	while (!_reader.eof()) {
		switch (_current) {
		case ' ':
		case '\t':
			nosave_and_next();
			break;

		case '\n':
		case '\r':
			new_line(false);
			break;

		case '\'':
		case '"':
			read_string();
			return TT_STRING;

		case '[':
			{
				nosave_and_next();
				int sep = count_nosave('=');
				if (_current == '[') {
					read_long_string(sep);
					return TT_STRING;
				} else {
					assert(sep == 0);
					_buff << '[';
					return TT_LEFT_SQUARE;
				}
			}

		case ']':
			save_and_next();
			return TT_RIGHT_SQUARE;

		case '(':
			save_and_next();
			return TT_LEFT_PARENTHESIS;

		case ')':
			save_and_next();
			return TT_RIGHT_PARENTHESIS;

		case '{':
			save_and_next();
			return TT_LEFT_BRACE;

		case '}':
			save_and_next();
			return TT_RIGHT_BRACE;

		case '^':
			save_and_next();
			return TT_EXP;

		case '*':
			save_and_next();
			return TT_MUL;

		case '/':
			save_and_next();
			return TT_DIV;

		case '%':
			save_and_next();
			return TT_MOD;

		case '+':
			save_and_next();
			return TT_PLUS;

		// �޷������Ǹ���,���Ǽ���
		case '-':
			nosave_and_next();
			if (_current == '-') {
				read_comment();
				return TT_COMMENT;
			} else {
				_buff << '-';
				return TT_MINUS;
			}

		case '<':
			save_and_next();
			if (_current == '=') {
				save_and_next();
				return TT_LESS_EQUAL;
			} else {
				return TT_LESS;
			}

		case '>':
			save_and_next();
			if (_current == '=') {
				save_and_next();
				return TT_GREATER_EQUAL;
			} else {
				return TT_GREATER;
			}

		case '=':
			save_and_next();
			if (_current == '=') {
				save_and_next();
				return TT_EQUAL;
			} else {
				return TT_ASSIGN;
			}

		case '~':
			save_and_next();
			assert(_current == '=');
			save_and_next();
			return TT_NOT_EQUAL;

		case '#':
			save_and_next();
			return TT_LEN;

		case ',':
			save_and_next();
			return TT_COMMA;

		case ';':
			save_and_next();
			return TT_SEMICOLON;

		case ':':
			nosave_and_next();
			if (_current != ':') {
				_buff << ":";
				return TT_COLON;
			} else {
				nosave_and_next();
				read_id();
				assert(_current == ':');
				nosave_and_next();
				assert(_current == ':');

				return TT_LABEL;
			}

		case '.':
			save_and_next();
			if (_current == '.') {
				save_and_next();
				if (_current == '.') {
					save_and_next();
					return TT_VARARG;
				} else {
					return TT_CONCAT;
				}
			} else if (!std::isdigit(_current)) {
				return TT_DOT;
			}

			// fallthrough
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			read_number();
			return TT_NUMBER;

		default:
			{
				read_id();
				std::map<std::string, TokenType>::iterator it =
						_s_reserved_tokens.find(_buff.str());
				if (it != _s_reserved_tokens.end()) {
					return it->second;
				}
				return TT_ID;
			}
		}
	}

	return TT_EOS;
}

static int hexvalue(char ch) {
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	}

	return std::tolower(ch) - 'a' + 10;
}

static int decvalue(char ch) {
	return ch - '0';
}

void Lexer::escape() {
	nosave_and_next();
	switch (_current) {
	// \xXX
	case 'x':
		{
			int ch = 0;
			for (int i = 0; i < 2; i++) {
				nosave_and_next();
				// assertӦ����Ӧ�ô�,��������ת����
				assert(std::isxdigit(_current));
				ch = ch * 16 + hexvalue(_current);
			}
			assert(ch <= 255);
			_buff << (char)ch;
		}
		break;

	// \uXXXX
	case 'u':
		{
			int code = 0;
			for (int i = 0; i < 4; i++) {
				nosave_and_next();
				assert(std::isxdigit(_current));
				code = code * 16 + decvalue(_current);
			}
			_buff << "(unicode: " << code << ")";
		}
		break;

	// '\'����
	case '\\':
		_buff << '\\';
		break;

	// \DDD
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		{
			int ch = 0;
			for (int i = 0; i < 3; i++) {
				nosave_and_next();
				assert(std::isdigit(_current));
				ch = ch * 10 + decvalue(_current);
			}
			assert(ch <= 255);
			_buff << (char)ch;
		}
		break;

	// ���ַ�ת��
	case 't':
		_buff << '\a';
		nosave_and_next();
		break;

	case 'r':
		_buff << '\r';
		nosave_and_next();
		break;

	case 'n':
		_buff << '\n';
		nosave_and_next();
		break;

	case '\'':
		_buff << '\'';
		nosave_and_next();
		break;

	case '"':
		_buff << '"';
		nosave_and_next();
		break;

	default:
		assert(false);
		break;
	}
}

void Lexer::read_string() {
	char sep = _current;
	nosave_and_next();

	while (!_reader.eof()) {
		switch (_current) {
		// ת��
		case '\\':
			escape();
			break;

		// ���ܵĽ���
		case '\'':
		case '"':
			if (_current == sep) {
				nosave_and_next();
				return;
			} else {
				save_and_next();
			}
			break;

		// ����
		case '\n':
		case '\r':
			assert(false);
			break;

		// ��ͨ�ַ�
		default:
			save_and_next();
			break;
		}
	}
}

void Lexer::read_long_string(int sep) {
	assert(_current == '[');
	nosave_and_next();

	if (_current == '\n' || _current == '\r') {
		new_line(false);
	}

	while (!_reader.eof()) {
		switch (_current) {
		case ']':
			nosave_and_next();
			// ��ע����,������ a[b[c]]�����,��Ҫ����a[b]=�����
			if (_current == ']' || _current == '=') {
				int nsep = count_nosave('=');
				if (_current == ']' && nsep == sep) {
					nosave_and_next();
					return;
				} else {
					_buff << "]" << std::string(nsep, '=');
					save_and_next();
				}
			}
			break;

		case '\n':
		case '\r':
			new_line(true);
			break;

		case '\\':
			escape();
			break;

		default:
			save_and_next();
			break;
		}
	}
	assert(false);
}

void Lexer::read_id() {
	save_and_next();

	while (!_reader.eof()) {
		if (std::isalnum(_current) || _current == '_') {
			save_and_next();
		} else {
			break;
		}
	}
}

static bool is_base_digit(int base, char ch) {
	assert(base >= 2 && base <= 36);

	if (std::isdigit(ch)) {
		if ((ch - '0') < base) {
			return true;
		} else {
			return false;
		}
	}

	int num = std::tolower(ch) - 'a' + 10;
	if (num >= 10 && num < base) {
		return true;
	} else {
		return false;
	}
}

// _buff�������fallthrough��'.'
void Lexer::read_number() {
	bool has_dot = !_buff.str().empty();
	bool has_exp = false;
	char exp = 'e';
	int base = 10;


	// 0.xx | 0oo | 0xXXX
	if (_current == '0') {
		save_and_next();
		if (_current == '.') {
			assert(!has_dot);
			has_dot = true;
			save_and_next();
		} else if (_current == 'x' || _current == 'X') {
			base = 16;
			exp = 'p';
			save_and_next();
		} else if (_current >= '0' && _current <= '7') {
			base = 8;
			save_and_next();
		} else {
			return;
		}
	}

	while (!_reader.eof()) {
		if (is_base_digit(base, _current)) {
			save_and_next();
		} else if (std::tolower(_current) == exp) {
			assert(!has_exp);
			has_exp = true;
			save_and_next();
		} else if (_current == '.') {
			assert(!has_dot);
			has_dot = true;
			save_and_next();
		} else {
			break;
		}
	}
}

void Lexer::read_comment() {
	nosave_and_next();

	// ��Ϊע����û���κ��﷨����,�����κ�������п���
	// ��Ҫ���� --[a] = b �� -- [=123 �����
	if (_current == '[') {
		nosave_and_next();
		int sep = count_nosave('=');
		if (_current == '[') {
			read_long_string(sep);
			return;
		} else {
			_buff << '[' << std::string(sep, '=');
		}
	}

	while (!_reader.eof()) {
		if (_current == '\n' || _current == '\r') {
			new_line(false);
			break;
		}

		save_and_next();
	}
}

} // namespace mpl

#if 0

#include <iostream>
#include "file_reader.h"

static void print_token(const ::mpl::Token& t) {
	std::cout << t.type << "\t" << t.text << std::endl;
}

int main() {
	::mpl::FileReader fr("test.txt");

	::mpl::Lexer lexer(fr);

	while (true) {
		::mpl::Token t = lexer.next();
		if (t.type == ::mpl::TT_EOS) {
			break;
		}

		print_token(t);
	}
}
#endif