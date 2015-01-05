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

// unionֻ�ܱ����������
union Value {
	void* v;
	bool b;
	double n;
	String* s;
	Function* f;
};

// ����value����,�ɸ���
// �о�û��Ҫ��ptr,ֻռ2��ָ���С,������shared_ptr��С
// ����Ҫ����shared_ptr��ʵ�����ü���
// �����ڲ���������ö�����Ptr
// ȱ����nil/bool/number����Ҫ������
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

	// s������Ȩ��ȫ����tvalue
	static TValuePtr create(String* s);
	static TValuePtr create(const char* s);
	static TValuePtr create(const std::string& s);

	// f������Ȩ��ȫ����tvalue
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

	// _is_ref/set/clone��Ҫ������������(��mpl��֧��)
	// ������˵:
	// �����õĸ�ֵʹ��set
	// ����һ��ֱ��create����
	// ��ֵ����ʱ,ע���ж�_is_ref��Ptr��unique
	bool _is_ref;

public:
	bool is_ref() const;
	void set_ref(bool is_ref);

	// ������ȫ��ͬ�Ķ�����TValue,��Ҫ�Լ�����������
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