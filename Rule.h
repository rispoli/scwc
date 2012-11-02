#ifndef RULE
#define RULE
#include "Compartment.h"
#include "OCompartment.h"
#include "PCompartment.h"
#include "Term.h"
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

class Rule {
	void add_to_map(const Term *, const Term *, std::map< const Term *, std::vector<const Term *> *> &);
	bool sfind(const std::string, const std::vector<std::string *> &);
	void build_lhs2rhs(const std::vector<const Term *> &, const std::vector<const Term *> &);
public:
	const std::vector<const Term *> &lhs;
	const std::vector<const Term *> &rhs;
	const double rate_function;
	const std::string &lhs_variable_symbol;
	const std::vector<std::string *> &rhs_variables_symbols;
	const bool more_than_one_compartment_in_lhs;
	bool at_least_one_compartment_in_rhs;
	bool throw_it_all_away;
	bool at_least_one_pcompartment_in_lhs;
	std::map< const Term *, std::vector<const Term *> * > lhs2rhs_on_level;

	Rule(const std::vector<const Term *> &, const std::vector<const Term *> &, const double, const std::string &, const std::vector<std::string *> &, const bool);
	~Rule();

	friend std::ostream& operator<<(std::ostream &, const Rule &);
};
#endif
