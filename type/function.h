#ifndef MPL_TYPE_FUNCTION_H
#define MPL_TYPE_FUNCTION_H

#include <vector>
#include <string>
#include <cstdint>
#include "value.h"

namespace mpl {
namespace type {

// 静态信息
class Function {
public:
	Function();
	~Function();

public:

private:
	std::vector<std::string> _args;
	std::vector<std::string> _locals;
	std::vector<std::string> _upvalues;
	std::vector<uint32_t> _opcodes;
};


// 动态信息
class Activation {
public:
	Activation();
	~Activation();

private:
	Function* _function;
	int _pc;
	std::vector<TValuePtr> _args;
	std::vector<TValuePtr> _locals;
	std::vector<TValuePtr> _upvalues;
};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_FUNCTION_H