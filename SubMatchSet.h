#ifndef SUBMATCHSET
#define SUBMATCHSET
#include "Compartment.h"
#include "Rule.h"
#include "SubMatch.h"
#include "Term.h"
#include "utilities.h"
#include <map>
#include <typeinfo>
#include <vector>

class MatchSet;
class Rule;

class SubMatchSet {
public:
	typedef std::map< const Term *, std::vector<SubMatch *> * > submatchsetType;
	submatchsetType submatchset;

	SubMatchSet() {}
	~SubMatchSet();

	multiplicityType submatch(const Rule &, std::vector<Term *> &, MatchSet *);
	multiplicityType submatch_rule(const Rule *, const Term &, std::vector<Term *> &, MatchSet *, std::vector<Term *> *);
	multiplicityType submatch_rule_term(const Rule *, const Term &, std::vector<Term *> &, Term &, MatchSet *, std::vector<Term *> *);

	friend std::ostream& operator<<(std::ostream &, const SubMatchSet &);
};

inline std::ostream& operator<<(std::ostream &os, const SubMatchSet &s) {
	for(SubMatchSet::submatchsetType::const_iterator it = s.submatchset.begin(); it != s.submatchset.end(); ++it) {
		os << "\t" << *it->first << ":\n";
		for(unsigned int i = 0; i < it->second->size(); ++i)
			os<< *(*it->second)[i];
		os << "\n";
	}
	return os;
}
#endif
