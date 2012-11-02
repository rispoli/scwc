#include "Gillespie.h"

CRandomSFMT0 *rng;

double rn(void) {
	return rng->Random();
}

std::pair<const Rule*, Match *> get_mu(MatchSet &m, double d) {
	double s = 0;
	for(MatchSet::matchsetType::const_iterator it = m.matchset.begin(); it != m.matchset.end(); ++it) {
		for(unsigned int i = 0; i < it->second->size(); ++i) {
			s += (*it->second)[i]->rate;
			if(d < s)
				return std::make_pair(it->first, (*it->second)[i]);
		}
	}
	return std::make_pair((Rule *)NULL, (Match *)NULL);
}

std::map<const Term *, SubMatch *> * get_updates(SubMatchSet &s) {
	std::map<const Term *, SubMatch *> *updates = new std::map<const Term *, SubMatch *>;
	for(SubMatchSet::submatchsetType::const_iterator it = s.submatchset.begin(); it != s.submatchset.end(); ++it) {
		if(it->second->size() == 1)
			(*updates)[it->first] = (*it->second)[0];
		else {
			multiplicityType sum = 0;
			for(unsigned int i = 0; i < it->second->size(); ++i)
				sum += (*it->second)[i]->multiplicity;

			multiplicityType d = rn() * sum;
			multiplicityType s = 0;
			unsigned int i = 0;
			for(; i < it->second->size(); ++i) {
				s += (*it->second)[i]->multiplicity;
				if(d < s) break;
			}
			(*updates)[it->first] = (*it->second)[i];
		}
	}
	return updates;
}

void update_or_insert(const Term &t, std::vector<Term *> &vt) {
	for(unsigned int i = 0; i < vt.size(); ++i) {
		if(*vt[i] == t) {
			vt[i]->update_add(t);
			return;
		} else if(*vt[i] > t) {
			Term *nt;
			if(typeid(t) == typeid(Atom))
				nt = new Atom(static_cast<const Atom &>(t));
			else
				nt = new Compartment(static_cast<const Compartment &>(t));
			vt.insert(vt.begin() + i, nt);
			return;
		}
	}
	Term *nt;
	if(typeid(t) == typeid(Atom))
		nt = new Atom(static_cast<const Atom &>(t));
	else
		nt = new Compartment(static_cast<const Compartment &>(t));
	vt.insert(vt.end(), nt);
}

std::vector<Term *> * get_copy(std::vector<Term *> &vt) {
	std::vector<Term *> *nvt = new std::vector<Term *>;
	for(unsigned int i = 0; i < vt.size(); ++i) {
		Term *nt;
		if(typeid(*vt[i]) == typeid(Atom))
			nt = new Atom(*static_cast<Atom *>(vt[i]));
		else
			nt = new Compartment(*static_cast<Compartment *>(vt[i]));
		nvt->push_back(nt);
	}
	return nvt;
}

void insert_in_order(std::vector<Term *> &vt, Term &t) {
	unsigned int i = 0;
	while(i < vt.size() && *vt[i] <= t) ++i;
	vt.insert(vt.begin() + i, &t);
}

void copy_in_term(std::vector<Term *> &where, std::vector<Term *> &what) {
	for(unsigned int i = 0; i < what.size(); ++i) {
		if(typeid(*what[i]) == typeid(Atom))
			update_or_insert(*static_cast<const Atom *>(what[i]), where);
		else
			insert_in_order(where, *new Compartment(*static_cast<Compartment *>(what[i])));
	}
}

void do_updates(const Rule &r, std::map<const Term *, SubMatch *> &updates) {
	std::vector<Term *> *term_to_update = NULL;
	std::vector<Term *> compartments_to_delete;
	std::map<const OCompartment *, std::vector<Term *> *> ocompartments2ground;
	instantiationType instantiations;
	for(std::map<const Term *, SubMatch *>::const_iterator it = updates.begin(); it != updates.end(); ++it) {
		const Term &t(*it->first);
		SubMatch &s(*it->second);
		term_to_update = s.parent;
		int position_to_delete = vfind(*s.parent, &s.term);
		if(s.term.update_delete(t) == 0 && typeid(t) != typeid(PCompartment)) {
			delete (*s.parent)[position_to_delete];
			s.parent->erase(s.parent->begin() + position_to_delete);
		}
		if(typeid(t) == typeid(PCompartment)) {
			Compartment *c = static_cast<Compartment *>(&s.term);
			const PCompartment *pc = static_cast<const PCompartment *>(&t);

			// reference everything in instantiations for possible uses
			instantiations[pc->wrap_variable_symbol] = &c->wrap;
			instantiations[pc->content_variable_symbol] = &c->content;

			compartments_to_delete.push_back(c);

			s.parent->erase(s.parent->begin() + position_to_delete);

			std::map< const Term *, std::vector<const Term *> * >::const_iterator it = r.lhs2rhs_on_level.find(&t);
			if(it != r.lhs2rhs_on_level.end()) {
				for(unsigned int i = 0; i < it->second->size(); ++i) {
					const OCompartment *rhs = static_cast<const OCompartment *>((*it->second)[i]);
					ocompartments2ground[rhs] = term_to_update;
				}
			}
		}
	}

	std::vector<Term *> *reference_for_instantiations = NULL;
	if(r.throw_it_all_away) {
		reference_for_instantiations = new std::vector<Term *>;
		for(unsigned int i = 0; i < term_to_update->size(); ++i)
			reference_for_instantiations->push_back((*term_to_update)[i]);
		term_to_update->clear();
	} else
		reference_for_instantiations = get_copy(*term_to_update);
	instantiations[r.lhs_variable_symbol] = reference_for_instantiations;

	// add terms in the rule's rhs
	for(unsigned int i = 0; i < r.rhs.size(); ++i)
		if(typeid(*r.rhs[i]) == typeid(Atom) || typeid(*r.rhs[i]) == typeid(Compartment))
			update_or_insert(*r.rhs[i], *term_to_update);
		else if(!r.at_least_one_pcompartment_in_lhs)
			insert_in_order(*term_to_update, *static_cast<const OCompartment *>(r.rhs[i])->ground(instantiations));

	bool lhs_variable_already_met = false;
	for(unsigned int i = 0; i < r.rhs_variables_symbols.size(); ++i) {
		if(*r.rhs_variables_symbols[i] != r.lhs_variable_symbol || lhs_variable_already_met) {
			instantiationType::const_iterator it = instantiations.find(*r.rhs_variables_symbols[i]);
			if(it != instantiations.end())
				copy_in_term(*term_to_update, *(instantiations[*r.rhs_variables_symbols[i]]));
		}
		if(*r.rhs_variables_symbols[i] == r.lhs_variable_symbol) lhs_variable_already_met = true;
	}

	// instantiations all available: build ground terms and add in place
	for(std::map<const OCompartment *, std::vector<Term *> *>::const_iterator it = ocompartments2ground.begin(); it != ocompartments2ground.end(); ++it)
		insert_in_order(*it->second, *it->first->ground(instantiations));

	// clean-up deleted compartments
	for(unsigned int i = 0; i < compartments_to_delete.size(); ++i)
		delete compartments_to_delete[i];

	for(unsigned int i = 0; i < reference_for_instantiations->size(); ++i)
		delete (*reference_for_instantiations)[i];
	delete reference_for_instantiations;

}

void gillespie(MatchSet &ma, double &time, CRandomSFMT0 *_rng) {

	rng = _rng;

	double tau = (1 / ma.rate_sum) * log(1 / rn());
	time += tau;
	std::pair<const Rule *, Match *> mu = get_mu(ma, rn() * ma.rate_sum);

	std::map<const Term *, SubMatch *> *updates = get_updates(mu.second->submatchset);
	do_updates(*mu.first, *updates);

	delete updates;

}
