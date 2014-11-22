#include "lexer.h"

#include <cassert>
#include "lexer/manual_lexer.h"
#include "lexer/auto_lexer.h"

namespace mpl {

std::shared_ptr<Lexer> create_lexer(const std::string& type, ::mpl::Reader& reader) {
	if (type == "manual") {
		return std::make_shared<::mpl::lexer::ManualLexer>(reader);
	} else if (type == "auto") {
		return std::make_shared<::mpl::lexer::AutoLexer>(reader);
	}
	assert(false);
	return std::shared_ptr<Lexer>();
}

} // namespace mpl