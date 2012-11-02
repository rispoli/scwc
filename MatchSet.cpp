#include "MatchSet.h"

MatchSet::~MatchSet() {
	for(matchsetType::const_iterator it = matchset.begin(); it != matchset.end(); ++it) {
		std::vector<Match *> &vs(*it->second);
		for(unsigned int i = 0; i < vs.size(); ++i)
			delete vs[i];
		delete &vs;
	}
}

void MatchSet::match(Model &m) {
	for(unsigned int i = 0; i < m.rules.size(); ++i)
		match_rule_term(*m.rules[i], m.term);

}

void MatchSet::match_rule_term(const Rule &r, std::vector<Term *> &vt) {
	SubMatchSet *submatchset = new SubMatchSet;
	multiplicityType multiplicity = submatchset->submatch(r, vt, this);
	if(multiplicity != -1) {

		double match_rate = multiplicity * r.rate_function;
		matchsetType::const_iterator it = matchset.find(&r);
		if(it != matchset.end()) {
			it->second->push_back(new Match(*submatchset, match_rate));
		} else {
			std::vector<Match *> *vs = new std::vector<Match *>;
			vs->push_back(new Match(*submatchset, match_rate));
			matchset[&r] = vs;
		}
		rate_sum += match_rate;

	} else // delete partially built submatch if match fails
		delete submatchset;
}
