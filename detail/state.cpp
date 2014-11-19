#include "state.h"

#include <cassert>

namespace mpl {
namespace detail {

// ---- Pred ----

Pred::Pred(const std::string& name) : _name(name), _neg(false) {

}

Pred::~Pred() {

}

void Pred::set_neg() {
	_neg = true;
}

void Pred::add(Byte min, Byte max) {
	assert(min <= max);
	_ranges.push_back(std::make_pair(min, max));
}

void Pred::add(Byte b) {
	_bytes.insert(b);
}

bool Pred::operator()(Byte b) const {
	bool is_in = _bytes.find(b) != _bytes.end();
	if (!is_in) {
		for (size_t i = 0; i < _ranges.size(); i++) {
			if (b >= _ranges[i].first && b <= _ranges[i].second) {
				is_in = true;
				break;
			}
		}
	}

	return _neg ? !is_in : is_in;
}

bool Pred::operator<(const Pred& p) const {
	return _name < p._name;
}
bool Pred::operator==(const Pred& p) const {
	return _name == p._name;
}

// must be init here
std::map<std::string, PredPtr> Pred::_s_preds;

PredPtr Pred::create(const std::string& name) {
	if (_s_preds.find(name) == _s_preds.end()) {
		_s_preds[name].reset(new Pred(name));
	}

	return _s_preds[name];
}

// ---- State ----

State::State() : _is_last(false) {

}

State::~State() {

}

bool State::is_last() const {
	return _is_last;
}

void State::set_last() {
	_is_last = true;
}

void State::add(Byte b, int s) {
	_bytes[b] = s;
}

void State::add(const PredPtr& pred, int s) {
	_preds[pred] = s;
}

void State::add(const Annotation& anno) {
	_annos.push_back(anno);
}

int State::operator[](Byte b) const {
	{
		std::map<Byte, int>::const_iterator it = _bytes.find(b);
		if (it != _bytes.end()) {
			return it->second;
		}
	}

	for (std::map<PredPtr, int>::const_iterator it = _preds.begin();
		it != _preds.end(); ++it) {
		if ((*it->first)(b)) {
			return it->second;
		}
	}

	return -1;
}

const std::map<Byte, int>& State::bytes() const {
	return _bytes;
}
const std::map<PredPtr, int>& State::preds() const {
	return _preds;
}

const std::vector<Annotation>& State::annos() const {
	return _annos;
}

// ---- StateList ----

StateList::StateList() {

}

StateList::~StateList() {

}

size_t StateList::size() const {
	return _states.size();
}
const State& StateList::operator[](size_t s) const {
	assert(s < _states.size());

	return _states[s];
}

State& StateList::operator[](size_t s) {
	assert(s < _states.size());

	return _states[s];
}

size_t StateList::new_state() {
	size_t last = _states.size();
	_states.resize(last + 1);

	return last;
}

} // namespace detail
} // namespace mpl