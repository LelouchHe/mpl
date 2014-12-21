#include "ll_parser_generator.h"

#include <cstring>
#include <cassert>
#include <algorithm>

#include "../string_reader.h"

namespace mpl {
namespace parser {

LLParserGenerator::LLParserGenerator() {

}

LLParserGenerator::~LLParserGenerator() {

}

bool LLParserGenerator::build(const char* parser_file, const char* parser_name) {
	if (!parse(parser_file)) {
		return false;
	}

	if (!build()) {
		return false;
	}

	if (!generate(parser_name)) {
		return false;
	}

	return true;
}

static bool is_next_state(const char* buf, size_t len) {
	if (len == 2 && std::strncmp(buf, "%%", len) == 0) {
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

bool LLParserGenerator::parse(const char* parser_file) {
	std::FILE* in = NULL;
	// in = std::fopen(lexer_file, "r");
	fopen_s(&in, parser_file, "r");

	ParserState state = INCLUDE;
	std::string name;
	std::string value;

	bool is_in_action = false;

	bool is_over = false;
	char buf[1024];

	while (!is_over && std::fgets(buf, sizeof (buf), in) != NULL) {
		size_t len = std::strlen(buf);
		// ¥Ì––,ø’––,◊¢ Õ¬‘»•
		if (len == 0 || buf[0] == '\n' || buf[0] == '-') {
			continue;
		}
		assert(buf[len - 1] == '\n');
		buf[len - 1] = '\0';
		len--;

		switch (state) {
		case INCLUDE:
			if (is_next_state(buf, len)) {
				state = DEFINITION;
			}
			break;

		case DEFINITION:
			if (is_next_state(buf, len)) {
				state = LAST;
			} else {
				if (buf[0] == '}') {
					assert(len == 1);
					assert(is_in_action);
					is_in_action = false;
				} else if (!is_in_action) {
					const char* begin = skip_delim(buf, " \t");
					const char* end = find_delim(begin, " \t:");

					name.assign(begin, end - begin);
					if (*end != ':') {
						end = skip_delim(end, " \t");
					}
					assert(*end == ':');
					end++;

					begin = skip_delim(end, " \t");
					assert(begin != NULL);
					if (buf[len - 1] == '{') {
						len--;
						is_in_action = true;
					}
					value.assign(begin, len - (begin - buf));
				}

				if (!is_in_action) {
					_definitions[name].push_back(value);
					if (std::find(_priorities.begin(), _priorities.end(), name) == _priorities.end()) {
						_priorities.push_back(name);
					}
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

bool LLParserGenerator::build() {
	size_t size = _priorities.size();
	for (size_t i = 0; i < size; i++) {
		const std::string& left = _priorities[i];
		const Definition& definition = _definitions[left];
		for (Definition::const_iterator it = definition.begin();
				it != definition.end(); ++it) {
			_grammar.add(left, *it);
		}
	}
	_grammar.build();

	return true;
}

bool LLParserGenerator::generate(const char* parser_name) {
	if (!generate_header(parser_name)) {
		return false;
	}

	if (!generate_source(parser_name)) {
		return false;
	}

	return true;
}

bool LLParserGenerator::generate_header(const char* parser_name) {
	std::string header_filename;
	header_filename.append("parser/");
	header_filename.append(parser_name);
	header_filename.append(".h");

	std::FILE* out = NULL;
	//out = fopen(header_filename.c_str(), "w");
	fopen_s(&out, header_filename.c_str(), "w");

	fprintf(out, "#ifndef MPL_PARSER_%s_H\n", parser_name);
	fprintf(out, "#define MPL_PARSER_%s_H\n", parser_name);

	fprintf(out, "#include \"../lexer.h\"\n");

	fprintf(out, "namespace mpl {\n");
	fprintf(out, "class Reader;\n");
	fprintf(out, "namespace parser {\n");

	fprintf(out, "class %s {\n", parser_name);

	fprintf(out, "public:\n");
	fprintf(out, "    %s(::mpl::Reader& reader);\n", parser_name);
	fprintf(out, "    ~%s();\n", parser_name);

	fprintf(out, "public:\n");
	fprintf(out, "    typedef ::mpl::Lexer Lexer;\n");
	fprintf(out, "    typedef Lexer::Token Token;\n");
	fprintf(out, "    typedef Token::TokenType TokenType;\n");
	fprintf(out, "    void parse();\n");

	fprintf(out, "private:\n");
	fprintf(out, "    Lexer _lexer;\n");

	fprintf(out, "};\n");

	fprintf(out, "} // namespace parser\n");
	fprintf(out, "} // namespace mpl\n");

	fprintf(out, "#endif // MPL_PARSER_%s_H\n", parser_name);

	std::fclose(out);
	return true;
}

bool LLParserGenerator::generate_source(const char* parser_name) {
	std::string header_filename;
	header_filename.append("parser/");
	header_filename.append(parser_name);
	header_filename.append(".cpp");

	std::FILE* out = NULL;
	//out = fopen(header_filename.c_str(), "w");
	fopen_s(&out, header_filename.c_str(), "w");

	fprintf(out, "#include \"%s.h\"\n", parser_name);
	fprintf(out, "#include <stack>\n");
	fprintf(out, "#include <vector>\n");
	fprintf(out, "#include <string>\n");
	fprintf(out, "#include <map>\n");
	fprintf(out, "#include <iostream>\n");
	fprintf(out, "#include <cassert>\n");

	fprintf(out, "namespace mpl {\n");
	fprintf(out, "namespace parser {\n");

	generate_gramma(out);

	fprintf(out, "%s::%s(::mpl::Reader& reader) :\n", parser_name, parser_name);
	fprintf(out, "        _lexer(reader) {\n");
	fprintf(out, "}\n");

	fprintf(out, "%s::~%s() {\n", parser_name, parser_name);
	fprintf(out, "}\n");

	generate_name(out);
	generate_parse(out, parser_name);

	fprintf(out, "} // namespace parser\n");
	fprintf(out, "} // namespace mpl\n");


	std::fclose(out);
	return true;
}

bool LLParserGenerator::generate_gramma(std::FILE* out) {
	size_t size = _grammar.size();

	const std::vector<std::string>& nonterminals = _grammar.nonterminals();
	
	fprintf(out, "static const std::vector<std::string> s_nonterminals = {");
	for (size_t i = 0; i < size; i++) {
		if (i % 5 == 0) {
			fprintf(out, "\n\t");
		}
		fprintf(out, "\"%s\", ", nonterminals[i].c_str());
	}
	fprintf(out, "\n};\n");

	const std::vector<::mpl::parser::grammar::LLGrammar::InnerRules>& all_rules = _grammar.rules();

	fprintf(out, "static const std::vector<std::vector<std::vector<int> > > s_rules = {\n");
	for (size_t left = 0; left < size; left++) {
		fprintf(out, "{\n");

		const ::mpl::parser::grammar::LLGrammar::InnerRules& rules = all_rules[left];
		for (size_t i = 0; i < rules.size(); i++) {
			fprintf(out, "\t{");

			const ::mpl::parser::grammar::LLGrammar::InnerRule& rule = rules[i];
			for (size_t j = 0; j < rule.size(); j++) {
				fprintf(out, "%d, ", rule[j]);
			}

			fprintf(out, "},\n");
		}

		fprintf(out, "},\n");
	}
	fprintf(out, "};\n");

	fprintf(out, "static const int s_start = %d;\n", _grammar.start());

	const std::vector<::mpl::parser::grammar::LLGrammar::Tran>& trans = _grammar.trans();
	fprintf(out, "static const std::vector<std::map<int, int> > s_trans = {\n");
	for (size_t left = 0; left < size; left++) {
		fprintf(out, "{\n");

		int num = 0;
		const ::mpl::parser::grammar::LLGrammar::Tran& tran = trans[left];
		fprintf(out, "\t");
		for (::mpl::parser::grammar::LLGrammar::Tran::const_iterator it = tran.begin();
				it != tran.end(); ++it) {
			if (num >= 5) {
				fprintf(out, "\n\t");
				num = 0;
			}
			fprintf(out, "{%d, %d}, ", (int)it->first, it->second);
			num++;
		}

		fprintf(out, "\n},\n");
	}
	fprintf(out, "};\n");

	return true;
}

bool LLParserGenerator::generate_name(std::FILE* out) {
	fprintf(out, "static const std::string& name(int token) {\n");
	
	fprintf(out, "    if (token >= 0) {\n");
	fprintf(out, "        return ::mpl::Lexer::token_name((::mpl::Lexer::TokenType)token);\n");
	fprintf(out, "    } else {\n");
	fprintf(out, "        token = -token;\n");
	fprintf(out, "        assert(token > 0 && (size_t)token < s_nonterminals.size());\n");
	fprintf(out, "        return s_nonterminals[token];\n");
	fprintf(out, "    }\n");

	fprintf(out, "}\n");

	return true;
}

bool LLParserGenerator::generate_parse(std::FILE* out, const char* parser_name) {
	fprintf(out, "void %s::parse() {\n", parser_name);

	fprintf(out, "    std::stack<int> st;\n");
	fprintf(out, "    st.push(s_start);\n");
	fprintf(out, "    Token current = _lexer.next();\n");
	fprintf(out, "    while (!st.empty() && current.type != TokenType::ERROR) {\n");
	fprintf(out, "        int token = st.top();\n");
	fprintf(out, "        st.pop();\n");
	fprintf(out, "        std::cout << \"expected: \" << name(token) << std::endl;");
	fprintf(out, "        if (token >= 0) {\n");
	fprintf(out, "            if ((TokenType)token == current.type) {\n");
	fprintf(out, "                std::cout << \"match: (\" << current.type << \", \" << current.text << \")\" << std::endl;\n");
	fprintf(out, "                current = _lexer.next();\n");
	fprintf(out, "            } else {\n");
	fprintf(out, "                std::cout << \"not match : (\" << current.type << \", \" << current.text << \")\" << std::endl;\n");
	fprintf(out, "                break;\n");
	fprintf(out, "            }\n");
	fprintf(out, "        } else {\n");
	fprintf(out, "            token = -token;\n");
	fprintf(out, "            const std::map<int, int>& tran = s_trans[token];\n");
	fprintf(out, "            std::map<int, int>::const_iterator it = tran.find(current.type);\n");
	fprintf(out, "            if (it == tran.end()) {\n");
	fprintf(out, "                std::cout << \"not match\" << std::endl;\n");
	fprintf(out, "                break;\n");
	fprintf(out, "            }\n");
	fprintf(out, "            const std::vector<int>& rule = s_rules[token][it->second];\n");
	fprintf(out, "            if (rule.empty()) {\n");
	fprintf(out, "                std::cout << \"match empty\" << std::endl;\n");
	fprintf(out, "                continue;\n");
	fprintf(out, "            }\n");
	fprintf(out, "            for (std::vector<int>::const_reverse_iterator rit = rule.rbegin(); rit != rule.rend(); ++rit) {\n");
	fprintf(out, "                st.push(*rit);\n");
	fprintf(out, "            }\n");
	fprintf(out, "        }\n");
	fprintf(out, "    }\n");
	
	fprintf(out, "}\n");
	return true;
}

} // namespace parser
} // namespace mpl

#if 0

int main() {
	const char* parser_file = "mpl.parser";
	const char* parser_name = "GeneratedLLParser";

	::mpl::parser::LLParserGenerator gen;
	gen.build(parser_file, parser_name);

	return 0;
}

#endif