#ifndef MPL_DETAIL_STATE_H
#define MPL_DETAIL_STATE_H

#include <memory>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <cstdint>

namespace mpl {
namespace detail {

typedef uint8_t Byte;
const Byte EPSILON = 0x0;

class Pred;
typedef std::shared_ptr<Pred> PredPtr;

class Pred {
public:
	~Pred();

	void set_neg();
	void add(Byte min, Byte max);
	void add(Byte b);

	bool operator()(Byte b) const;
	bool operator<(const Pred& p) const;
	bool operator==(const Pred& p) const;

	// name will be [^$name],
	static PredPtr create(const std::string& name);

private:
	std::string _name;
	bool _neg;
	std::vector<std::pair<Byte, Byte> > _ranges;
	std::set<Byte> _bytes;

	Pred(const std::string& name);

	static std::map<std::string, PredPtr> _s_preds;
};

enum AnnotationType {
	NONE = 0,
	TOKEN_TYPE,
	LEFT_CATCH,
	RIGHT_CATCH
};

class Annotation {
public:
	Annotation() : type(NONE), value(-1) {}
	~Annotation() {}

public:
	AnnotationType type;
	int value;
};

class State {
public:
	State();
	~State();

	bool is_last() const;
	void set_last();

	void add(Byte b, int s);
	void add(const PredPtr& pred, int s);
	void add(const Annotation& anno);

	// -1: error
	int operator[](Byte b) const;

	// use operator[] to get next state
	// those two below are for iterator
	const std::map<Byte, int>& bytes() const;
	const std::map<PredPtr, int>& preds() const;
	
	const std::vector<Annotation>& annos() const;

private:
	bool _is_last;

	std::map<Byte, int> _bytes;
	std::map<PredPtr, int> _preds;

	std::vector<Annotation> _annos;
};

class StateList {
public:
	StateList();
	~StateList();

	size_t size() const;
	const State& operator[](size_t s) const;
	State& operator[](size_t s);

	// return new index in states
	size_t new_state();

private:
	std::vector<State> _states;
};

} // namespace detail
} // namespace mpl

#endif // MPL_DETAIL_STATE_H