#ifndef MPL_AST_PARSER_NODE_H
#define MPL_AST_PARSER_NODE_H

#include <memory>
#include <vector>
#include "../lexer.h"

namespace mpl {
namespace ast {

class ParserNode;
typedef std::shared_ptr<ParserNode> ParserNodePtr;

class ParserNode {
public:
	~ParserNode();

public:
	typedef ::mpl::Lexer::Token Token;

	static ParserNodePtr create(const Token& token);


	void add(const ParserNodePtr& node);

	size_t size() const;
	const ParserNodePtr& operator[](size_t s) const;
	const Token& token() const;

	void debug() const;

private:
	void debug(std::vector<bool>* pis_last) const;

private:
	Token _token;
	std::vector<ParserNodePtr> _children;

	ParserNode(const Token& token);
	ParserNode(const ParserNode&);
	ParserNode& operator=(const ParserNode&);
};
	
} // namespace ast
} // namespace mpl

#endif // MPL_AST_PARSER_NODE_H