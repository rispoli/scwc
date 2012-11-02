#include "MatchSet.h"
#include "SubMatchSet.h"

SubMatchSet::~SubMatchSet() {
	for(submatchsetType::const_iterator it = submatchset.begin(); it != submatchset.end(); ++it) {
		std::vector<SubMatch *> &vs(*it->second);
		for(unsigned int i = 0; i < vs.size(); ++i)
			delete vs[i];
		delete &vs;
	}
}

multiplicityType SubMatchSet::submatch(const Rule &rule, std::vector<Term *> &term, MatchSet *m) {
	std::vector<Term *> rec_calls;
	multiplicityType multiplicity = 1;
	for(unsigned int i = 0; i < rule.lhs.size(); ++i) {
		multiplicityType sub_multiplicity = submatch_rule(&rule, *rule.lhs[i], term, m, &rec_calls);
		multiplicity *= sub_multiplicity;
	}
	if(submatchset.size() >= rule.lhs.size()) return multiplicity;
	else return -1;
}

multiplicityType SubMatchSet::submatch_rule(const Rule *rule, const Term &r, std::vector<Term *> &term, MatchSet *m, std::vector<Term *> *rec_calls) {
	multiplicityType multiplicity = 1;
	for(unsigned int j = 0; j < term.size(); ++j) {
		multiplicityType sub_multiplicity = submatch_rule_term(rule, r, term, *term[j], m, rec_calls);
		if(sub_multiplicity != -1)
			multiplicity *= sub_multiplicity;
	}
	return multiplicity;
}

multiplicityType SubMatchSet::submatch_rule_term(const Rule *rule, const Term &r, std::vector<Term *> &term, Term &t, MatchSet *m, std::vector<Term *> *rec_calls) {

	multiplicityType multiplicity = r.match(t, this);

	if(multiplicity != -1) {

		SubMatch *sm = new SubMatch(t, &term, multiplicity);

		submatchsetType::const_iterator it = submatchset.find(&r);
		if(it != submatchset.end()) {
			it->second->push_back(sm);
		} else {
			std::vector<SubMatch *> *vs = new std::vector<SubMatch *>;
			vs->push_back(sm);
			submatchset[&r] = vs;
		}
	}

	if(rule != NULL && m != NULL && rec_calls != NULL && typeid(t) == typeid(Compartment) && vfind(*rec_calls, &t) == -1) {
		Compartment &c(static_cast<Compartment &>(t));
		if(!rule->at_least_one_compartment_in_rhs)
			m->match_rule_term(*rule, c.wrap);
		m->match_rule_term(*rule, c.content);
		rec_calls->push_back(&t);
	}
	return multiplicity;
}
