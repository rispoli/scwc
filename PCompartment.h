#ifndef PCOMPARTMENT
#define PCOMPARTMENT
#include "Atom.h"
#include "Compartment.h"
#include "SubMatchSet.h"
#include "Term.h"
#include <vector>
#include <string>

class PCompartment : public Compartment {
public:
	const std::string &wrap_variable_symbol;
	const std::string &content_variable_symbol;

	PCompartment(std::vector<Term *> &, std::vector<const Term *> &, const std::string &, const std::string &);
	~PCompartment();

	virtual multiplicityType match(const Term &, SubMatchSet *) const;

protected:
	virtual std::ostream& write(std::ostream &) const;
};
#endif
