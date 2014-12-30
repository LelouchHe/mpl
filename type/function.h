#ifndef MPL_TYPE_FUNCTION_H
#define MPL_TYPE_FUNCTION_H

namespace mpl {
namespace type {

class Proto;

// 静态信息
class Proto {

};

// 动态信息
class Function {
public:
	Function();
	~Function();

public:

private:
	Proto* _p;

};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_FUNCTION_H