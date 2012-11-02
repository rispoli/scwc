#ifndef SUBMATCH
#define SUBMATCH
#include "Term.h"
#include <vector>

class SubMatch {
public:
	Term &term;
	std::vector<Term *> *parent;
	multiplicityType multiplicity;

	SubMatch(Term &t, std::vector<Term *> *p, multiplicityType m) : term(t), parent(p), multiplicity(m) {}

	friend std::ostream& operator<<(std::ostream &, const SubMatch &);
};

inline std::ostream& operator<<(std::ostream &os, const SubMatch &s) {
	os << "\t\t" << s.term << ", parent = " << s.parent << ", multiplicity = " << s.multiplicity;
	return os << "\n";
}
#endif
