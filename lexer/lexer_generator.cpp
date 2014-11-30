#include "lexer_generator.h"

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace mpl {
namespace lexer {

LexerGenerator::LexerGenerator() {
	reset();

	_privates.push_back("TokenType lex();");
	_privates.push_back("TokenType tag_type(int tag);");
	_privates.push_back("::mpl::Reader& _reader;");
	_privates.push_back("std::ostringstream _buf;");
	_privates.push_back("char _current;");
	_privates.push_back("Token _next;");
	_privates.push_back("Token _ahead;");
}

LexerGenerator::~LexerGenerator() {

}

void LexerGenerator::reset() {
	_includes.clear();
	_globals.clear();
	_inits.clear();
	_definitions.clear();
	_priorities.clear();
	_actions.clear();

	_includes.push_back("#include <sstream>");
	_includes.push_back("#include \"detail/state.h\"");
}

bool LexerGenerator::build(const char* lexer_file, const char* lexer_name) {
	reset();

	if (!parse(lexer_file)) {
		return false;
	}

	if (!build()) {
		return false;
	}

	if (!generate(lexer_name)) {
		return false;
	}

	return true;
}

bool LexerGenerator::build() {
	_merger.reset();

	size_t size = _priorities.size();
	for (size_t i = 0; i < size; i++) {
		_generator.parse((::mpl::lexer::detail::Byte *)_definitions[_priorities[i]].c_str(), i);
		_merger.add(_generator);
	}

	_merger.build();
	_generator.build(_merger, true);

	return true;
}

static bool is_next_state(const char* buf, size_t len) {
	if (len == 2 && strncmp(buf, "%%", len) == 0) {
		return true;
	}

	return false;
}

static const char* skip_delim(const char* buf, const char* delim) {
	const char* begin = buf;
	while (*begin != '\0' && (std::strchr(delim, *begin) != NULL)) {
		begin++;
	}

	return begin;
}

static const char* find_delim(const char* buf, const char* delim) {
	const char* begin = buf;
	while (*begin != '\0' && (std::strchr(delim, *begin) == NULL)) {
		begin++;
	}

	return begin;
}

bool LexerGenerator::parse(const char* lexer_file) {
	std::FILE* in = NULL;
	// in = std::fopen(lexer_file, "r");
	fopen_s(&in, lexer_file, "r");

	LexerState state = START;
	std::string name;
	std::string value;
	Statment stat;
	bool is_continue = false;

	bool is_over = false;
	char buf[1024];
	// fgets最多读n-1字符,最后一个字符永远是'\0','\n'会在一行末尾
	while (!is_over && std::fgets(buf, sizeof (buf), in) != NULL) {
		size_t len = std::strlen(buf);
		// 错行,空行,注释略去
		if (len == 0 || buf[0] == '\n' || buf[0] == '-') {
			continue;
		}
		assert(buf[len - 1] == '\n');
		buf[len - 1] = '\0';
		len--;

		switch (state) {
		case START:
			if (is_next_state(buf, len)) {
				state = GLOBAL;
			} else {
				_includes.push_back(buf);
			}

			break;

		case GLOBAL:
			if (is_next_state(buf, len)) {
				state = INIT;
			} else {
				_globals.push_back(buf);
			}

			break;

		case INIT:
			if (is_next_state(buf, len)) {
				state = DEFINITION;
			} else {
				_inits.push_back(buf);
			}

			break;

		case DEFINITION:
			if (is_next_state(buf, len)) {
				assert(!is_continue);
				state = ACTION;
			} else {
				const char* begin = skip_delim(buf, " \t");
				if (is_continue) {
					if (buf[len - 1] == '\\') {
						len--;
					} else {
						is_continue = false;
					}
					value.append(begin, len - (begin - buf));
				} else {
					const char* end = find_delim(begin, " \t:");

					name.assign(begin, end - begin);
					if (*end != ':') {
						end = skip_delim(end, " \t");
					}
					assert(*end == ':');
					end++;

					begin = skip_delim(end, " \t");
					assert(begin != NULL);

					if (buf[len - 1] == '\\') {
						len--;
						is_continue = true;
					}
					value.assign(begin, len - (begin - buf));
				}
				if (!is_continue) {
					_definitions[name] = value;
					_priorities.push_back(name);
				}
			}

			break;

		case ACTION:
			if (is_next_state(buf, len)) {
				assert(!is_continue);
				state = LAST;
			} else {
				if (buf[0] == '}') {
					assert(len == 1);
					assert(is_continue);
					is_continue = false;
				} else if (is_continue) {
					stat.push_back(buf);
				} else {
					const char* begin = skip_delim(buf, " \t");
					const char* end = find_delim(begin, " \t:");

					name.assign(begin, end - begin);
					begin = find_delim(end, ":");
					assert(begin != NULL);

					begin = skip_delim(begin, " \t:");
					assert(*begin == '{');
					begin++;


					if (buf[len - 1] == '}') {
						begin = skip_delim(begin, " \t");
						begin--;
						buf[begin - buf] = '\t';
						len--;
					} else {
						is_continue = true;
					}

					if (*begin != '\0') {
						stat.push_back(std::string(begin, len - (begin - buf)));
					}
				}
				if (!is_continue) {
					_actions[name] = stat;
					stat.clear();
				}
			}

			break;

		case LAST:
		default:
			is_over = true;
			break;
		}
	}
	assert(state == LAST);

	std::fclose(in);
	return true;
}

bool LexerGenerator::generate(const char* lexer_name) {
	if (!generate_header(lexer_name)) {
		return false;
	}

	if (!generate_source(lexer_name)) {
		return false;
	}

	return true;
}

bool LexerGenerator::generate_header(const char* lexer_name) {
	std::string header_filename;
	header_filename.append("lexer/");
	header_filename.append(lexer_name);
	header_filename.append(".h");

	std::FILE* out = NULL;
	//out = fopen(header_filename.c_str(), "w");
	fopen_s(&out, header_filename.c_str(), "w");

	fprintf(out, "#ifndef MPL_LEXER_%s_H\n", lexer_name);
	fprintf(out, "#define MPL_LEXER_%s_H\n", lexer_name);

	for (size_t i = 0; i < _includes.size(); i++) {
		fprintf(out, "%s\n", _includes[i].c_str());
	}

	fprintf(out, "namespace mpl {\n");
	fprintf(out, "class Reader;\n");
	fprintf(out, "namespace lexer {\n");

	fprintf(out, "class %s {\n", lexer_name);
	
	fprintf(out, "public:\n");
	fprintf(out, "    %s(::mpl::Reader& reader);\n", lexer_name);
	fprintf(out, "    ~%s();\n", lexer_name);

	fprintf(out, "public:\n");

	generate_token(out);

	fprintf(out, "    bool run();\n");
	fprintf(out, "    const Token& next();\n");
	fprintf(out, "    const Token& lookahead();\n");
	fprintf(out, "    static TokenType token_type(const std::string& name);\n");

	if (!_globals.empty()) {
		fprintf(out, "public:\n");
		for (size_t i = 0; i < _globals.size(); i++) {
			fprintf(out, "    %s\n", _globals[i].c_str());
		}
	}

	if (!_actions.empty()) {
		fprintf(out, "public:\n");
		for (std::map<std::string, Statment>::iterator it = _actions.begin();
				it != _actions.end(); ++it) {
			fprintf(out, "    void %s_action(Token& token);\n", it->first.c_str());
		}
	}

	if (!_privates.empty()) {
		fprintf(out, "private:\n");
		for (size_t i = 0; i < _privates.size(); i++) {
			fprintf(out, "    %s\n", _privates[i].c_str());
		}
	}
	
	fprintf(out, "};\n");

	generate_token_comparison(out, lexer_name);

	fprintf(out, "} // namespace lexer\n");
	fprintf(out, "} // namespace mpl\n");

	fprintf(out, "#endif // MPL_LEXER_%s_H\n", lexer_name);

	std::fclose(out);
	return true;
}

bool LexerGenerator::generate_token(std::FILE* out) {
	size_t size = _priorities.size();
	assert(size == _definitions.size());

	fprintf(out, "\tenum TokenType {");

	for (size_t i = 0; i < size; i++) {
		if (i % 5 == 0) {
			fprintf(out, "\n\t\t");
		}
		fprintf(out, "TT_%s", _priorities[i].c_str());
		if (i == 0) {
			fprintf(out, " = 0");
		}
		fprintf(out, ", ");
	}

	fprintf(out, "\n\t\t");
	fprintf(out, "EOS, EPSILON, NONTERMINAL, ERROR, SKIP, LAST_TOKEN\n");

	fprintf(out, "\t};\n");

	fprintf(out, "\tclass Token {\n");
	fprintf(out, "\tpublic:\n");
	fprintf(out, "\t    typedef TokenType TokenType;\n");
	fprintf(out, "\t    TokenType type;\n");
	fprintf(out, "\t    std::string text;\n");
	fprintf(out, "\t    Token() : type(EOS) {}\n");
	fprintf(out, "\t    Token(TokenType atype, const std::string& atext) :\n");
	fprintf(out, "\t            type(atype), text(atext) {}\n");
	fprintf(out, "\t};\n");

	return true;
}

bool LexerGenerator::generate_token_comparison(std::FILE* out, const char* lexer_name) {
	fprintf(out, "inline bool operator==(const %s::Token& a, const %s::Token& b) {\n", lexer_name, lexer_name);
	fprintf(out, "    if (a.type != b.type) {\n");
	fprintf(out, "        return false;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    if (a.type != %s::TokenType::NONTERMINAL) {\n", lexer_name);
	fprintf(out, "        return true;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    return a.text == b.text;\n");
	fprintf(out, "}\n");

	fprintf(out, "inline bool operator<(const %s::Token& a, const %s::Token& b) {\n", lexer_name, lexer_name);
	fprintf(out, "    if (a.type < b.type) {\n");
	fprintf(out, "        return true;\n");
	fprintf(out, "    } else if (a.type > b.type) {\n");
	fprintf(out, "        return false;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    if (a.type != %s::TokenType::NONTERMINAL) {\n", lexer_name);
	fprintf(out, "        return false;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    return a.text < b.text;\n");
	fprintf(out, "}\n");

	fprintf(out, "inline bool operator<=(const %s::Token& a, const %s::Token& b) {\n", lexer_name, lexer_name);
	fprintf(out, "    return a < b || a == b;\n");
	fprintf(out, "}\n");

	return true;
}

bool LexerGenerator::generate_source(const char* lexer_name) {
	std::string source_filename;
	source_filename.append("lexer/");
	source_filename.append(lexer_name);
	source_filename.append(".cpp");

	std::FILE* out = NULL;
	//out = fopen(source_filename.c_str(), "w");
	fopen_s(&out, source_filename.c_str(), "w");

	fprintf(out, "#include \"%s.h\"\n", lexer_name);
	fprintf(out, "#include <cassert>\n");
	fprintf(out, "#include \"../reader.h\"\n");

	fprintf(out, "namespace mpl {\n");
	fprintf(out, "namespace lexer {\n");

	fprintf(out, "%s::%s(::mpl::Reader& reader) : \n", lexer_name, lexer_name);
	fprintf(out, "        _reader(reader), _current('\\0') {\n");

	for (size_t i = 0; i < _inits.size(); i++) {
		fprintf(out, "    %s\n", _inits[i].c_str());
	}

	fprintf(out, "}\n");

	fprintf(out, "%s::~%s() {}\n", lexer_name, lexer_name);

	generate_dfa(out);

	generate_token_types(out, lexer_name);

	generate_action(out, lexer_name);

	generate_interface(out, lexer_name);

	std::fclose(out);
	return true;
}

bool LexerGenerator::generate_dfa(std::FILE* out) {

	::mpl::lexer::detail::DFA& dfa = _generator;

	generate_trans(out, dfa);

	generate_tags(out, dfa);

	generate_start_last(out, dfa);

	return true;
}

bool LexerGenerator::generate_trans(std::FILE* out, const ::mpl::lexer::detail::DFA& dfa) {
	fprintf(out, "using ::mpl::lexer::detail::DFATran;\n");
	fprintf(out, "static const std::vector<DFATran> s_trans = {\n");

	size_t size = dfa.size();
	for (size_t i = 0; i < size; i++) {
		const ::mpl::lexer::detail::DFATran& tran = dfa[i];
		fprintf(out, "{\n\t");
		int num = 0;
		for (::mpl::lexer::detail::DFATran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			if (num >= 5) {
				fprintf(out, "\n\t");
				num = 0;
			}
			fprintf(out, "{0x%x, %d}, ", it->first, it->second);
			num++;
		}
		fprintf(out, "\n},\n");
	}

	fprintf(out, "};\n");

	return true;
}

// 由于vs本身的bug(http://stackoverflow.com/questions/19269345/initializing-map-of-maps-with-initializer-list-in-vs-2013)
// 此处简化tag,直接把最小的输出即可
bool LexerGenerator::generate_tags(std::FILE* out, const ::mpl::lexer::detail::DFA& dfa) {
	fprintf(out, "static const std::map<size_t, int> s_tags = {\n");

	size_t size = dfa.size();
	fprintf(out, "\t");
	int num = 0;
	for (size_t i = 0; i < size; i++) {
		const ::mpl::lexer::detail::Tag& tag = dfa.tags(i);
		if (tag.empty()) {
			continue;
		}
		if (num >= 5) {
			fprintf(out, "\n\t");
			num = 0;
		}

		fprintf(out, "{ %u, %d }, ", i, *tag.begin());
		num++;
	}

	fprintf(out, "\n};\n");
	return true;
}

bool LexerGenerator::generate_start_last(std::FILE* out, const ::mpl::lexer::detail::DFA& dfa) {
	fprintf(out, "static const int s_start = %d;\n", dfa.start());

	const ::mpl::lexer::detail::States& last = dfa.last();
	fprintf(out, "using ::mpl::lexer::detail::States;\n");
	fprintf(out, "static const States s_last = {\n");

	fprintf(out, "\t");
	int num = 0;
	for (::mpl::lexer::detail::States::const_iterator it = last.begin();
			it != last.end(); ++it) {
		if (num >= 5) {
			fprintf(out, "\n\t");
			num = 0;
		}
		fprintf(out, "%d, ", *it);
		num++;
	}
	fprintf(out, "\n");

	fprintf(out, "};\n");
	return true;
}

bool LexerGenerator::generate_token_types(std::FILE* out, const char* lexer_name) {
	size_t size = _priorities.size();
	assert(size == _definitions.size());

	fprintf(out, "static const std::map<std::string, %s::TokenType> s_token_types = {", lexer_name);

	for (size_t i = 0; i < size; i++) {
		if (i % 2 == 0) {
			fprintf(out, "\n\t");
		}
		fprintf(out, "{ \"%s\", %s::TokenType::TT_%s }, ",
				_priorities[i].c_str(), lexer_name, _priorities[i].c_str());
	}

	fprintf(out, "\n};\n");
	return true;
}

bool LexerGenerator::generate_action(std::FILE* out, const char* lexer_name) {
	fprintf(out, "typedef void (%s::*ActionType)(%s::Token&);\n", lexer_name, lexer_name);
	fprintf(out, "static const std::map<%s::TokenType, ActionType> s_actions = {\n", lexer_name);
	
	for (std::map<std::string, Statment>::iterator it = _actions.begin();
			it != _actions.end(); ++it) {
		fprintf(out, "\t{ %s::TT_%s, &%s::%s_action },\n", lexer_name, it->first.c_str(), lexer_name, it->first.c_str());
	}
	
	fprintf(out, "};\n");

	for (std::map<std::string, Statment>::iterator it = _actions.begin();
			it != _actions.end(); ++it) {
		fprintf(out, "void %s::%s_action(%s::Token& token) {\n", lexer_name, it->first.c_str(), lexer_name);
		Statment& statment = it->second;
		for (size_t i = 0; i < statment.size(); i++) {
			fprintf(out, "%s\n", statment[i].c_str());
		}
		fprintf(out, "}\n");
	}

	return true;
}

bool LexerGenerator::generate_interface(std::FILE* out, const char* lexer_name) {
	generate_tag_type(out, lexer_name);
	generate_lex(out, lexer_name);
	generate_run(out, lexer_name);
	generate_token_type(out, lexer_name);

	fprintf(out, "const %s::Token& %s::next() {\n", lexer_name, lexer_name);
	fprintf(out, "    if (_ahead.type != EOS) {\n");
	fprintf(out, "        _next = _ahead;\n");
	fprintf(out, "        _ahead.type = EOS;\n");
	fprintf(out, "        return _next;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    _next.type = lex();\n");
	fprintf(out, "    _next.text = _buf.str();\n");
	fprintf(out, "    return _next;\n");
	fprintf(out, "}\n");

	fprintf(out, "const %s::Token& %s::lookahead() {\n", lexer_name, lexer_name);
	fprintf(out, "    if (_ahead.type == EOS) {\n");
	fprintf(out, "        _ahead.type = lex();\n");
	fprintf(out, "        _ahead.text = _buf.str();\n");
	fprintf(out, "    }\n");
	fprintf(out, "    return _ahead;\n");
	fprintf(out, "}\n");

	fprintf(out, "} // namespace lexer\n");
	fprintf(out, "} // namespace mpl\n");

	return true;
}

bool LexerGenerator::generate_tag_type(std::FILE* out, const char* lexer_name) {
	fprintf(out, "%s::TokenType %s::tag_type(int tag) {\n", lexer_name, lexer_name);

	fprintf(out, "    TokenType type = (TokenType)tag;\n");
	fprintf(out, "    std::map<TokenType, ActionType>::const_iterator it = s_actions.find(type);\n");
	fprintf(out, "    if (it == s_actions.end()) {\n");
	fprintf(out, "        return type;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    Token token;\n");
	fprintf(out, "    token.type = type;\n");
	fprintf(out, "    token.text = _buf.str();\n");
	fprintf(out, "    (this->*(it->second))(token);\n");
	fprintf(out, "    _buf.str(token.text);\n");
	fprintf(out, "    return token.type;\n");

	fprintf(out, "}\n");

	return true;
}

bool LexerGenerator::generate_lex(std::FILE* out, const char* lexer_name) {
	fprintf(out, "%s::TokenType %s::lex() {\n", lexer_name, lexer_name);

	fprintf(out, "    _buf.str(\"\");\n");
	fprintf(out, "    int pre = -1;\n");
	fprintf(out, "    int cur = s_start;\n");
	fprintf(out, "    if (_current == '\\0') {\n");
	fprintf(out, "        _current = _reader.next();\n");
	fprintf(out, "    }\n");
	fprintf(out, "    while (!_reader.eof()) {\n");
	fprintf(out, "        if (s_last.find(cur) != s_last.end()) {\n");
	fprintf(out, "            pre = cur;\n");
	fprintf(out, "        }\n");
	fprintf(out, "        const DFATran& tran = s_trans[cur];\n");
	fprintf(out, "        DFATran::const_iterator it = tran.find(_current);\n");
	fprintf(out, "        if (it == tran.end() || it->second == -1) {\n");
	fprintf(out, "            if (it == tran.end()) {\n");
	fprintf(out, "                it = tran.find(::mpl::lexer::detail::OTHER);\n");
	fprintf(out, "            }\n");
	fprintf(out, "            if (it == tran.end() || it->second == -1) {\n");
	fprintf(out, "                cur = -1;\n");
	fprintf(out, "                break;\n");
	fprintf(out, "            }\n");
	fprintf(out, "        }\n");
	fprintf(out, "        cur = it->second;\n");
	fprintf(out, "        _buf << _current;\n");
	fprintf(out, "        _current = _reader.next();\n");
	fprintf(out, "    }\n");
	fprintf(out, "    if (s_last.find(cur) != s_last.end()) {\n");
	fprintf(out, "        pre = cur;\n");
	fprintf(out, "    }\n");
	fprintf(out, "    if (pre == -1) {\n");
	fprintf(out, "        if (_reader.eof()) {\n");
	fprintf(out, "            _buf << \"$\";\n");
	fprintf(out, "            return EOS;\n");
	fprintf(out, "        } else {\n");
	fprintf(out, "            return ERROR;\n");
	fprintf(out, "        }\n");
	fprintf(out, "    }\n");
	fprintf(out, "    std::map<size_t, int>::const_iterator it = s_tags.find(pre);\n");
	fprintf(out, "    assert(it != s_tags.end());\n");
	fprintf(out, "    TokenType type = tag_type(it->second);\n");
	fprintf(out, "    if (type == SKIP) {\n");
	fprintf(out, "        return lex();\n");
	fprintf(out, "    } else {\n");
	fprintf(out, "        return type;\n");
	fprintf(out, "    }\n");

	fprintf(out, "}\n");
	return true;
}

bool LexerGenerator::generate_run(std::FILE* out, const char* lexer_name) {
	fprintf(out, "bool %s::run() {\n", lexer_name);

	fprintf(out, "    while (true) {\n");
	fprintf(out, "        const Token& token = next();\n");
	fprintf(out, "        if (token.type == ERROR) {\n");
	fprintf(out, "            return false;\n");
	fprintf(out, "        } else if (token.type == EOS) {\n");
	fprintf(out, "            break;\n");
	fprintf(out, "        }\n");
	fprintf(out, "    }\n");
	fprintf(out, "    return true;\n");

	fprintf(out, "}\n");
	return true;
}

bool LexerGenerator::generate_token_type(std::FILE* out, const char* lexer_name) {
	fprintf(out, "%s::TokenType %s::token_type(const std::string& name) {\n", lexer_name, lexer_name);

	fprintf(out, "    std::map<std::string, TokenType>::const_iterator it =\n");
	fprintf(out, "            s_token_types.find(name);\n");
	fprintf(out, "    if (it != s_token_types.end()) {\n");
	fprintf(out, "        return it->second;\n");
	fprintf(out, "    } else {\n");
	fprintf(out, "        return TokenType::ERROR;\n");
	fprintf(out, "    }\n");

	fprintf(out, "}\n");
	return true;
}
	
} // namespace lexer
} // namespace mpl

#if 0

int main() {
	const char* lexer_file = "mpl.lexer";
	const char* lexer_name = "GeneratedLexer";

	::mpl::lexer::LexerGenerator gen;
	gen.build(lexer_file, lexer_name);

	return 0;
}

#endif