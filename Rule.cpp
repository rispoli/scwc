#include "Rule.h"

void Rule::add_to_map(const Term *key, const Term *value, std::map< const Term *, std::vector<const Term *> *> &where) {
	std::map< const Term *, std::vector<const Term *> *>::const_iterator it = where.find(key);
	if(it != where.end()) {
		it->second->push_back(value);
	} else {
		std::vector<const Term *> *vt = new std::vector<const Term *>;
		vt->push_back(value);
		where[key] = vt;
	}
}

bool Rule::sfind(const std::string s, const std::vector<std::string *> &st) {
	for(unsigned int i = 0; i < st.size(); ++i)
		if(s == *st[i]) return true;
	return false;
}

void Rule::build_lhs2rhs(const std::vector<const Term *> &lhs, const std::vector<const Term *> &rhs) {
	bool first_for_this_depth = true;
	const PCompartment *key = NULL;
	for(unsigned int i = 0; i < lhs.size(); ++i)
		if(typeid(*lhs[i]) == typeid(PCompartment)) {
			at_least_one_pcompartment_in_lhs = true;
			const PCompartment &lhs_i(static_cast<const PCompartment &>(*lhs[i]));
			for(unsigned int j = 0; j < rhs.size(); ++j)
				if(typeid(*rhs[j]) == typeid(OCompartment)) {
					const OCompartment &rhs_j(static_cast<const OCompartment &>(*rhs[j]));
					if(first_for_this_depth) key = &lhs_i;
					add_to_map(key, &rhs_j, lhs2rhs_on_level);
				}
			first_for_this_depth = false;
		}
}

Rule::Rule(const std::vector<const Term *> &_lhs, const std::vector<const Term *> &_rhs, const double r, const std::string &_lv, const std::vector<std::string *> &_rv, const bool m) : lhs(_lhs), rhs(_rhs), rate_function(r), lhs_variable_symbol(_lv), rhs_variables_symbols(_rv), more_than_one_compartment_in_lhs(m), at_least_one_compartment_in_rhs(false), throw_it_all_away(true), at_least_one_pcompartment_in_lhs(false) {
	build_lhs2rhs(lhs, rhs);
	for(unsigned int j = 0; j < rhs.size(); ++j)
		if(typeid(*rhs[j]) == typeid(Compartment) || typeid(*rhs[j]) == typeid(OCompartment)) at_least_one_compartment_in_rhs = true;
	throw_it_all_away = !sfind(lhs_variable_symbol, rhs_variables_symbols);
}

Rule::~Rule() {
	for(unsigned int i = 0; i < lhs.size(); ++i)
		delete lhs[i];
	delete &lhs;
	for(unsigned int i = 0; i < rhs.size(); ++i)
		delete rhs[i];
	delete &rhs;
	for(std::map< const Term *, std::vector<const Term *> *>::const_iterator it = lhs2rhs_on_level.begin(); it != lhs2rhs_on_level.end(); ++it)
		delete it->second;
	delete &lhs_variable_symbol;
	for(unsigned int i = 0; i < rhs_variables_symbols.size(); ++i)
		delete rhs_variables_symbols[i];
	delete &rhs_variables_symbols;
}

std::ostream& operator<<(std::ostream &os, const Rule &r) {
	for(unsigned int i = 0; i < r.lhs.size(); ++i)
		os << *r.lhs[i] << " ";
	os << r.lhs_variable_symbol << " >>>[" << std::fixed << r.rate_function << "]>>> ";
	for(unsigned int i = 0; i < r.rhs.size(); ++i)
		os << *r.rhs[i] << " ";
	for(unsigned int i = 0; i < r.rhs_variables_symbols.size(); ++i)
		os << *r.rhs_variables_symbols[i] << " ";
	return os;
}
