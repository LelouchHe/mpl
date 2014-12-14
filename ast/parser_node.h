#ifndef MPL_AST_PARSER_NODE_H
#define MPL_AST_PARSER_NODE_H

#include <memory>
#include <vector>
#include "../parser.h"


namespace mpl {
namespace ast {

class ParserNode;
typedef std::shared_ptr<ParserNode> ParserNodePtr;
class ParserNode {
public:
	/*
	ParserNode();
	~ParserNode();
	*/

public:
	typedef ::mpl::Lexer::Token Token;

	void add(ParserNodePtr node);

	static ParserNodePtr create(Token token);

private:
	Token _token;
	std::vector<ParserNodePtr> _children;

	ParserNode();
	~ParserNode();
	ParserNode(const ParserNode&);
	ParserNode& operator=(const ParserNode&);
};
	
} // namespace ast
} // namespace mpl

#endif // MPL_AST_PARSER_NODE_H