#ifndef MATCHSET
#define MATCHSET
#include "Match.h"
#include "Model.h"
#include "Rule.h"
#include "Term.h"
#include <map>
#include <vector>

class MatchSet {
public:
	typedef std::map< const Rule *, std::vector<Match *> * > matchsetType;
	matchsetType matchset;
	double rate_sum;

	MatchSet() : rate_sum(0) {}
	~MatchSet();

	void match(Model &);
	void match_rule_term(const Rule &, std::vector<Term *> &);

	friend std::ostream& operator<<(std::ostream &, const MatchSet &);
};

inline std::ostream& operator<<(std::ostream &os, const MatchSet &m) {
	os << "Σ rates: " << std::fixed << m.rate_sum << "\n";
	for(MatchSet::matchsetType::const_iterator it = m.matchset.begin(); it != m.matchset.end(); ++it) {
		os << *it->first << "\n";
		for(unsigned int i = 0; i < it->second->size(); ++i)
			os<< *(*it->second)[i];
	}
	return os;
}
#endif
