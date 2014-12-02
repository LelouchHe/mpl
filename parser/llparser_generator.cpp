#include "llparser_generator.h"

#include <cstdio>
#include <cstring>
#include <cassert>

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

	return generate(parser_name);
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

bool LLParserGenerator::parse(const char* parser_file) {
	std::FILE* in = NULL;
	// in = std::fopen(lexer_file, "r");
	fopen_s(&in, parser_file, "r");

	ParserState state = START;
	std::string name;
	std::string value;

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
		case START:
			if (is_next_state(buf, len)) {
				state = DEFINITION;
			}
			break;

		case DEFINITION:
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

				_definitions[name].push_back(value);
				if (_priorities.empty() || _priorities.back() != name) {
					_priorities.push_back(name);
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
	return true;
}

bool LLParserGenerator::generate(const char* parser_name) {
	return true;
}

} // namespace parser
} // namespace mpl

#if 1

int main() {
	const char* parser_file = "mpl.parser";
	const char* parser_name = "GeneratedLLParser";

	::mpl::parser::LLParserGenerator gen;
	gen.build(parser_file, parser_name);

	return 0;
}

#endif