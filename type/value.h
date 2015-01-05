#ifndef MPL_TYPE_TVALUE_H
#define MPL_TYPE_TVALUE_H

#include <memory>
#include <string>

namespace mpl {
namespace type {

class String;
class Function;

enum Type {
	VT_NIL = 0,
	VT_BOOL,
	VT_NUMBER,
	VT_STRING,
	VT_FUNCTION,
};

// union只能保存基本类型
union Value {
	void* v;
	bool b;
	double n;
	String* s;
	Function* f;
};

// 带有value语义,可复制
// 感觉没必要成ptr,只占2个指针大小,甚至比shared_ptr还小
// 还是要引入shared_ptr来实现引用计数
// 程序内部对象的引用都是用Ptr
// 缺点是nil/bool/number都需要这样了
class TValue;
typedef std::shared_ptr<TValue> TValuePtr;
typedef std::weak_ptr<TValue> TValueWeakPtr;

class TValue {
public:
	~TValue();

public:
	static TValuePtr create();
	static TValuePtr create(bool b);
	static TValuePtr create(int n);
	static TValuePtr create(double n);

	// s的所有权完全交由tvalue
	static TValuePtr create(String* s);
	static TValuePtr create(const char* s);
	static TValuePtr create(const std::string& s);

	// f的所有权完全交由tvalue
	static TValuePtr create(Function* f);
	static TValuePtr create(Type type, Value value);

	Type type() const;
	Value value() const;

	operator bool() const;
	double number() const;
	String* string() const;
	Function* function() const;

private:
	Type _type;
	Value _value;

	// _is_ref/set/clone主要用于引用语义(但mpl不支持)
	// 具体来说:
	// 被引用的赋值使用set
	// 其他一律直接create即可
	// 赋值操作时,注意判断_is_ref和Ptr的unique
	bool _is_ref;

public:
	bool is_ref() const;
	void set_ref(bool is_ref);

	// 返回完全相同的独立的TValue,需要自己控制生命期
	TValue* clone() const;
	Value clone_value() const;

	void set();
	void set(bool b);
	void set(int n);
	void set(double n);

	void set(String* s);
	void set(const char* s);
	void set(const std::string& s);

	void set(Function* f);
	void set(Type type, Value value);

private:
	void clear();

	TValue();
	TValue(bool b);
	TValue(int n);
	TValue(double n);

	TValue(String* s);
	TValue(const char* s);
	TValue(const std::string& s);
	
	TValue(Function* f);
	TValue(Type type, Value value);
};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_TVALUE_H