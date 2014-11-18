#ifndef MPL_DFA_CONVERTER_H
#define MPL_DFA_CONVERTER_H

#include <vector>
#include <map>

namespace mpl {

class DFAConverter {
public:
	typedef std::map<char, int> DFATran;

	DFAConverter();
	~DFAConverter();

	bool parse(const char* str);

	// Pre: s < size()
	const DFATran& operator[](size_t s) const;
	size_t size() const;

	// < 0: ×´Ì¬´íÎó
	int start() const;
	const std::vector<int>& last() const;

private:
	int new_state();
	void reset();

private:
	std::vector<DFATran> _trans;
	
	int _start;
	std::vector<int> _last;
};

} // namespace mpl

#endif // MPL_DFA_CONVERTER_H