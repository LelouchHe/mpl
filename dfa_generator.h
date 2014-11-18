#ifndef MPL_DFA_GENERATOR_H
#define MPL_DFA_GENERATOR_H

#include <vector>
#include <map>
#include <set>

namespace mpl {

class DFAConverter;

class DFAGenerator {
public:
	typedef std::map<char, int> DFATran;

	DFAGenerator();
	~DFAGenerator();

	bool parse(const char* str);

	// Pre: s < size()
	const DFATran& operator[](size_t s) const;
	size_t size() const;

	// < 0: ×´Ì¬´íÎó
	int start() const;
	const std::vector<int>& last() const;

	bool full_match(const char* str) const;
	bool partial_match(const char* str, char** end) const;

private:
	void build(const ::mpl::DFAConverter& dfa, const std::set<std::vector<int> >& t);
	int new_state();
	void reset();

private:
	std::vector<DFATran> _trans;

	int _start;
	std::vector<int> _last;
};

} // namespace mpl

#endif // MPL_DFA_GENERATOR_H