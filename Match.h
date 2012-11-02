#ifndef MATCH
#define MATCH
#include "SubMatchSet.h"

class Match {
public:
	SubMatchSet &submatchset;
	double rate;


	Match(SubMatchSet &_submatchset, double _rate) : submatchset(_submatchset), rate(_rate) {}
	~Match() { delete &submatchset; }

	friend std::ostream& operator<<(std::ostream &, const Match &);
};

inline std::ostream& operator<<(std::ostream &os, const Match &m) {
	return os << "multiplicity = " << std::fixed << m.rate << ":\n" << m.submatchset;
}
#endif
