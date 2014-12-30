#ifndef MPL_TYPE_TVALUE_H
#define MPL_TYPE_TVALUE_H

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
	void* p;
	bool b;
	double n;
	String* s;
	Function* f;
};

// ����value����,�ɸ���
// �о�û��Ҫ��ptr,ֻռ2��ָ���С,������shared_ptr��С
class TValue {
public:
	TValue();
	TValue(bool b);
	TValue(int n);
	TValue(double n);
	TValue(String* s);
	TValue(Function* f);
	TValue(Type type, Value value);

	~TValue();

public:
	Type type() const;
	Value value() const;

	operator bool() const;
	double number() const;
	String* string() const;
	Function* function() const;

	void set();
	void set(bool b);
	void set(int n);
	void set(double n);
	void set(String* s);
	void set(Function* f);
	void set(Type type, Value value);

	TValue& operator=(bool b);
	TValue& operator=(int n);
	TValue& operator=(double n);
	TValue& operator=(String* s);
	TValue& operator=(Function* f);

private:
	Type _type;
	Value _value;
};

} // namespace type
} // namespace mpl

#endif // MPL_TYPE_TVALUE_H