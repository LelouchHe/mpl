#include "lalr_parser_generator.h"

namespace mpl {
namespace parser {

LALRParserGenerator::LALRParserGenerator() {

}

LALRParserGenerator::~LALRParserGenerator() {

}

bool LALRParserGenerator::build(const char* parser_file, const char* parser_name) {
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

// 前后追赶
static std::vector<std::string> split(const std::string& str, char delim) {
	std::vector<std::string> strs;

	size_t begin = 0;
	size_t end = begin;
	while (end < str.size()) {
		if (str[end] == delim) {
			while (begin < end && str[begin] == delim) {
				begin++;
			}
			if (begin != end) {
				strs.push_back(str.substr(begin, end - begin));
				begin = end;
			}
		}
		end++;
	}
	while (begin < end && str[begin] == delim) {
		begin++;
	}
	if (begin != end) {
		strs.push_back(str.substr(begin, end - begin));
	}

	return strs;
}

bool LALRParserGenerator::parse(const char* parser_file) {
	std::FILE* in = NULL;
	// in = std::fopen(lexer_file, "r");
	fopen_s(&in, parser_file, "r");

	ParserState state = INCLUDE;
	std::string name;
	std::string value;

	Statement action;
	bool is_in_action = false;

	bool is_over = false;
	char buf[1024];

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
		case INCLUDE:
			if (is_next_state(buf, len)) {
				state = DEFINITION;
			} else {
				_includes.push_back(buf);
			}
			break;

		case DEFINITION:
			if (is_next_state(buf, len)) {
				assert(!is_in_action);
				state = OPERATOR;
			} else {
				if (buf[0] == '}') {
					assert(len == 1);
					assert(is_in_action);
					is_in_action = false;
				} else if (is_in_action) {
					action.push_back(buf);
				} else {
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
					if (action.empty()) {
						_definitions[name].push_back(std::make_pair(value, -1));
					} else {
						_actions.push_back(action);
						_definitions[name].push_back(std::make_pair(value, _actions.size() - 1));
					}
					
					action.clear();
					if (std::find(_priorities.begin(), _priorities.end(), name) == _priorities.end()) {
						_priorities.push_back(name);
					}
				}
			}
			break;

		case OPERATOR:
			if (is_next_state(buf, len)) {
				state = LAST;
			} else {
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

				value.assign(begin, len - (begin - buf));

				std::vector<std::string> values = split(value, ' ');
				assert(values.size() == 2);

				int priority = strtol(values[0].c_str(), NULL, 0);
				::mpl::parser::grammar::LALRGrammar::Associativity associativity =
						::mpl::parser::grammar::LALRGrammar::Associativity::NONE;
				if (values[1] == "LEFT") {
					associativity = ::mpl::parser::grammar::LALRGrammar::Associativity::LEFT;
				} else if (values[1] == "RIGHT") {
					associativity = ::mpl::parser::grammar::LALRGrammar::Associativity::LEFT;
				}

				_attrs[name] = std::make_pair(priority, associativity);
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

bool LALRParserGenerator::build() {
	size_t size = _priorities.size();
	for (size_t i = 0; i < size; i++) {
		const std::string& left = _priorities[i];
		const Definitions& definitions = _definitions[left];
		for (size_t j = 0; j < definitions.size(); j++) {
			_grammar.add(left, definitions[j].first, definitions[j].second);
		}
	}

	for (std::map<std::string, Attribute>::const_iterator it = _attrs.begin();
			it != _attrs.end(); ++it) {
		_grammar.add(it->first, it->second.first, it->second.second);
	}
	_grammar.build();

	// _grammar.debug();

	return true;
}

bool LALRParserGenerator::generate(const char* parser_name) {
	if (!generate_header(parser_name)) {
		return false;
	}

	if (!generate_source(parser_name)) {
		return false;
	}

	return true;
}

bool LALRParserGenerator::generate_header(const char* parser_name) {
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
	fprintf(out, "#include \"../ast/parser_node.h\"\n");

	for (size_t i = 0; i < _includes.size(); i++) {
		fprintf(out, "%s\n", _includes[i].c_str());
	}

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
	fprintf(out, "    ::mpl::ast::ParserNodePtr build(bool is_debug = false);\n");

	fprintf(out, "private:\n");
	fprintf(out, "    Lexer _lexer;\n");

	fprintf(out, "};\n");

	fprintf(out, "} // namespace parser\n");
	fprintf(out, "} // namespace mpl\n");

	fprintf(out, "#endif // MPL_PARSER_%s_H\n", parser_name);

	std::fclose(out);
	return true;
}

bool LALRParserGenerator::generate_source(const char* parser_name) {
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
	generate_action(out);

	fprintf(out, "%s::%s(::mpl::Reader& reader) :\n", parser_name, parser_name);
	fprintf(out, "        _lexer(reader) {\n");
	fprintf(out, "}\n");

	fprintf(out, "%s::~%s() {\n", parser_name, parser_name);
	fprintf(out, "}\n");

	generate_build(out, parser_name);

	fprintf(out, "} // namespace parser\n");
	fprintf(out, "} // namespace mpl\n");

	std::fclose(out);
	return true;
}


bool LALRParserGenerator::generate_gramma(std::FILE* out) {
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

	const std::vector<::mpl::parser::grammar::LALRGrammar::InnerRules>& all_rules = _grammar.rules();

	fprintf(out, "static const std::vector<std::vector<std::pair<std::vector<int>, int> > > s_rules = {\n");
	for (size_t left = 0; left < size; left++) {
		fprintf(out, "{\n");

		const ::mpl::parser::grammar::LALRGrammar::InnerRules& rules = all_rules[left];
		for (size_t i = 0; i < rules.size(); i++) {
			fprintf(out, "\t{ {");

			const ::mpl::parser::grammar::LALRGrammar::InnerRule& rule = rules[i];
			for (size_t j = 0; j < rule.size(); j++) {
				fprintf(out, "%d, ", rule[j]);
			}

			fprintf(out, "}, %d },\n", _grammar.action(-(int)left, i));
		}

		fprintf(out, "},\n");
	}
	fprintf(out, "};\n");

	fprintf(out, "static const int ACCEPT = %d;\n", _grammar.ACCEPT);
	fprintf(out, "static const int SHIFT = %d;\n", _grammar.SHIFT);

	const std::vector<::mpl::parser::grammar::LALRGrammar::Tran>& trans = _grammar.trans();
	fprintf(out, "static const std::vector<std::map<int, std::pair<int, int> > > s_trans = {\n");
	for (size_t left = 0; left < trans.size(); left++) {
		fprintf(out, "{\n");

		int num = 0;
		const ::mpl::parser::grammar::LALRGrammar::Tran& tran = trans[left];
		fprintf(out, "\t");
		for (::mpl::parser::grammar::LALRGrammar::Tran::const_iterator it = tran.begin();
			it != tran.end(); ++it) {
			if (num >= 5) {
				fprintf(out, "\n\t");
				num = 0;
			}
			fprintf(out, "{%d, { %d, %d } }, ", (int)it->first, it->second.first, it->second.second);
			num++;
		}

		fprintf(out, "\n},\n");
	}
	fprintf(out, "};\n");

	return true;
}

bool LALRParserGenerator::generate_action(std::FILE* out) {
	for (size_t i = 0; i < _actions.size(); i++) {
		fprintf(out, "static void action_%d(\n"
					 "        const ::mpl::ast::ParserNodePtr& left,\n"
					 "        const std::vector< ::mpl::ast::ParserNodePtr>& right) {\n", i);
		for (size_t j = 0; j < _actions[i].size(); j++) {
			fprintf(out, "%s\n", _actions[i][j].c_str());
		}
		fprintf(out, "}\n");
	}
	fprintf(out, "static const std::vector< ::mpl::ast::ReduceAction> s_actions = {");
	for (size_t i = 0; i < _actions.size(); i++) {
		if (i % 5 == 0) {
			fprintf(out, "\n\t");
		}
		fprintf(out, "action_%d, ", i);
	}
	fprintf(out, "\n};\n");

	return true;
}

bool LALRParserGenerator::generate_build(std::FILE* out, const char* parser_name) {
	fprintf(out, "::mpl::ast::ParserNodePtr %s::build(bool is_debug) {\n", parser_name);

	fprintf(out, "    std::stack<size_t> st;\n");
	fprintf(out, "    std::vector< ::mpl::ast::ParserNodePtr> nodes;\n");
	fprintf(out, "    st.push(0);\n");

	fprintf(out, "    Token token(TokenType::EPSILON, \"\");\n");
	fprintf(out, "    while (!st.empty()) {\n");
	fprintf(out, "        size_t state = st.top();\n");
	fprintf(out, "        if (is_debug) {\n");
	fprintf(out, "            std::cout << \"state: \" << state << \"\\t\";\n");
	fprintf(out, "        }\n");
	fprintf(out, "        const std::map<int, std::pair<int, int> >& tran = s_trans[state];\n");
	fprintf(out, "        bool should_next = false;\n");
	fprintf(out, "        if (token.type == TokenType::EPSILON) {\n");
	fprintf(out, "            token = _lexer.lookahead();\n");
	fprintf(out, "            should_next = true;\n");
	fprintf(out, "        }\n");
	fprintf(out, "        if (is_debug) {\n");
	fprintf(out, "            std::cout << \"token: \" << token.text << \"\\t\";\n");
	fprintf(out, "        }\n");
	fprintf(out, "        std::map<int, std::pair<int, int> >::const_iterator it = tran.find(token.type);\n");
	fprintf(out, "        assert(it != tran.end());\n");
	fprintf(out, "        if (it->second.first < 0) {\n");
	fprintf(out, "            int left = it->second.first;\n");
	fprintf(out, "            int rule_no = it->second.second;\n");
	fprintf(out, "            token.type = (TokenType)left;\n");
	fprintf(out, "            token.text = s_nonterminals[-left];\n");
	fprintf(out, "            if (is_debug) {\n");
	fprintf(out, "                std::cout << \"reduce: \" << token.text << std::endl;\n");
	fprintf(out, "            }\n");
	fprintf(out, "            ::mpl::ast::ParserNodePtr parent = ::mpl::ast::ParserNode::create(token);\n");
	fprintf(out, "            const std::vector<int>& rule = s_rules[-left][rule_no].first;\n");
	fprintf(out, "            size_t children_start = nodes.size() - rule.size();\n");
	fprintf(out, "            for (size_t i = 0; i < rule.size(); i++) {\n");
	fprintf(out, "                st.pop();\n");
	fprintf(out, "                parent->add(nodes[children_start + i]);\n");
	fprintf(out, "            }\n");
	fprintf(out, "            nodes.resize(children_start);\n");
	fprintf(out, "            int action = s_rules[-left][rule_no].second;\n");
	fprintf(out, "            if (action != -1) {\n");
	fprintf(out, "                parent->reduce(s_actions[action]);\n");
	fprintf(out, "            }\n");
	fprintf(out, "            nodes.push_back(parent);\n");
	fprintf(out, "        } else if (it->second.first == ACCEPT) {\n");
	fprintf(out, "            if (is_debug) {\n");
	fprintf(out, "                std::cout << \"accept\" << std::endl;\n");
	fprintf(out, "            }\n");
	fprintf(out, "            break;\n");
	fprintf(out, "        } else {\n");
	fprintf(out, "            st.push(it->second.second);\n");
	fprintf(out, "            if (is_debug) {\n");
	fprintf(out, "                std::cout << \"shift: \" << it->second.second << std::endl;\n");
	fprintf(out, "            }\n");
	fprintf(out, "            if (token.type > 0) {\n");
	fprintf(out, "                nodes.push_back(::mpl::ast::ParserNode::create(token));\n");
	fprintf(out, "            }\n");
	fprintf(out, "            token.type = TokenType::EPSILON;\n");
	fprintf(out, "            token.text = \"\";\n");
	fprintf(out, "            if (should_next) {\n");
	fprintf(out, "                _lexer.next();\n");
	fprintf(out, "            }\n");
	fprintf(out, "        }\n");
	fprintf(out, "    }\n");

	fprintf(out, "    if (nodes.size() == 1) {\n");
	fprintf(out, "        return nodes[0];\n");
	fprintf(out, "    } else {\n");
	fprintf(out, "        return ::mpl::ast::ParserNodePtr();\n");
	fprintf(out, "    }\n");

	fprintf(out, "}\n");
	
	return true;
}

} // namespace parser
} // namespace mpl

#if 0

int main() {
	// const char* parser_file = "mpl.parser.always_list.lua";
	const char* parser_file = "mpl.parser.lua";

	const char* parser_name = "GeneratedLALRParser";

	::mpl::parser::LALRParserGenerator gen;
	gen.build(parser_file, parser_name);

	return 0;
}

#endif