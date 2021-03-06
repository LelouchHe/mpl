#include "manual_lexer.h"

#include <cctype>
#include <cassert>
#include "../reader.h"

namespace mpl {
namespace lexer {

ManualLexer::ManualLexer(::mpl::Reader& reader) :
		_reader(reader),
		_current('\0'), _line_num(0) {
}

ManualLexer::~ManualLexer() {

}

const ManualLexer::Token& ManualLexer::next() {
	if (_ahead.type != EOS) {
		_next = _ahead;
		_ahead.type = EOS;
		return _next;
	}

	_next.type = lex();
	_next.text = _buf.str();

	return _next;
}

// 连续lookahead,返回相同值
const ManualLexer::Token& ManualLexer::lookahead() {
	if (_ahead.type == EOS) {
		_ahead.type = lex();
		_ahead.text = _buf.str();
	}

	return _ahead;
}

ManualLexer::TokenType ManualLexer::token_type(const std::string& name) {
	std::map<std::string, TokenType>::const_iterator it =
			TOKEN_TYPES.find(name);
	if (it != TOKEN_TYPES.end()) {
		return it->second;
	} else {
		return TokenType::ERROR;
	}
}

static const std::string& token_name(TokenType type) {
	if ((size_t)type >= TOKEN_NAMES.size()) {
		type = TokenType::ERROR;
	}

	return TOKEN_NAMES[type];
}

bool ManualLexer::eof() {
	return _reader.eof();
}

void ManualLexer::new_line(bool should_save) {
	char ch = _reader.next();
	if (_current == '\r') {
		if (ch == '\n') {
			// "\r\n" -> windows
			if (should_save) {
				_buf << "\r\n";
			}
			_current = _reader.next();
		} else {
			// "\r" -> Mac
			if (should_save) {
				_buf << '\r';
			}
			_current = ch;
		}
	} else {
		// "\n" -> linux
		if (should_save) {
			_buf << '\n';
		}
		_current = ch;
	}

	_line_num++;
}

// 保存当前值,并取下一个值
void ManualLexer::save_and_next() {
	_buf << _current;
	_current = _reader.next();
}

void ManualLexer::nosave_and_next() {
	_current = _reader.next();
}

int ManualLexer::count_nosave(char ch) {
	int n = 0;
	while (_current == ch) {
		n++;
		nosave_and_next();
	}

	return n;
}

// lex将得到的字符传到_buf,并返回类型,不处理_next/_ahead
// _current保存下一个字符,或者'\0',表示尚没有读取
ManualLexer::TokenType ManualLexer::lex() {
	_buf.str("");

	if (_current == '\0') {
		_current = _reader.next();
	}

	while (!eof()) {
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
					_buf << '[';
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

		// 无法区分是负号,还是减号
		case '-':
			nosave_and_next();
			if (_current == '-') {
				read_comment();
				return TT_COMMENT;
			} else {
				_buf << '-';
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
			save_and_next();
			if (_current != ':') {
				return TT_COLON;
			} else {
				save_and_next();
				read_id();
				assert(_current == ':');
				save_and_next();
				assert(_current == ':');
				save_and_next();

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
				std::map<std::string, TokenType>::const_iterator it =
						TOKEN_RE_KEYS.find(_buf.str());
				if (it != TOKEN_RE_KEYS.end()) {
					return it->second;
				}
				return TT_ID;
			}
		}
	}

	_buf << "$";
	return EOS;
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

void ManualLexer::escape() {
	nosave_and_next();
	switch (_current) {
	// \xXX
	case 'x':
		{
			int ch = 0;
			for (int i = 0; i < 2; i++) {
				nosave_and_next();
				// assert应该在应用处,而不是在转换处
				assert(std::isxdigit(_current));
				ch = ch * 16 + hexvalue(_current);
			}
			assert(ch <= 255);
			_buf << (char)ch;
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
			_buf << "(unicode: " << code << ")";
		}
		break;

	// '\'本身
	case '\\':
		_buf << '\\';
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
			_buf << (char)ch;
		}
		break;

	// 单字符转义
	case 't':
		_buf << '\a';
		nosave_and_next();
		break;

	case 'r':
		_buf << '\r';
		nosave_and_next();
		break;

	case 'n':
		_buf << '\n';
		nosave_and_next();
		break;

	case '\'':
		_buf << '\'';
		nosave_and_next();
		break;

	case '"':
		_buf << '"';
		nosave_and_next();
		break;

	default:
		assert(false);
		break;
	}
}

void ManualLexer::read_string() {
	char sep = _current;
	nosave_and_next();

	while (!eof()) {
		switch (_current) {
		// 转义
		case '\\':
			escape();
			break;

		// 可能的结束
		case '\'':
		case '"':
			if (_current == sep) {
				nosave_and_next();
				return;
			} else {
				save_and_next();
			}
			break;

		// 换行
		case '\n':
		case '\r':
			assert(false);
			break;

		// 普通字符
		default:
			save_and_next();
			break;
		}
	}
}

void ManualLexer::read_long_string(int sep) {
	assert(_current == '[');
	nosave_and_next();

	if (_current == '\n' || _current == '\r') {
		new_line(false);
	}

	while (!eof()) {
		switch (_current) {
		case ']':
			nosave_and_next();
			// 在注释中,不考虑 a[b[c]]的情况,但要考虑a[b]=的情况
			if (_current == ']' || _current == '=') {
				int nsep = count_nosave('=');
				if (_current == ']' && nsep == sep) {
					nosave_and_next();
					return;
				} else {
					_buf << "]" << std::string(nsep, '=');
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

void ManualLexer::read_id() {
	save_and_next();

	while (!eof()) {
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

// _buf里可能有fallthrough的'.'
void ManualLexer::read_number() {
	bool has_dot = !_buf.str().empty();
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

	while (!eof()) {
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

void ManualLexer::read_comment() {
	nosave_and_next();

	// 因为注释中没有任何语法规则,所以任何情况都有可能
	// 需要考虑 --[a] = b 或 -- [=123 的情况
	if (_current == '[') {
		nosave_and_next();
		int sep = count_nosave('=');
		if (_current == '[') {
			read_long_string(sep);
			return;
		} else {
			_buf << '[' << std::string(sep, '=');
		}
	}

	while (!eof()) {
		if (_current == '\n' || _current == '\r') {
			new_line(false);
			break;
		}

		save_and_next();
	}
}

} // namespace lexer
} // namespace mpl

#if 0

#include <iostream>
#include "../file_reader.h"

static void print_token(const ::mpl::lexer::ManualLexer::Token& t) {
	std::cout << t.type << "\t" << t.text << std::endl;
}

int main() {
	::mpl::FileReader fr("test.txt");

	::mpl::lexer::ManualLexer lexer(fr);

	while (true) {
		const ::mpl::lexer::ManualLexer::Token& t = lexer.next();
		if (t.type == ::mpl::lexer::ManualLexer::TokenType::EOS) {
			break;
		}

		print_token(t);
	}
}

#endif