#ifndef OCOMPARTMENT
#define OCOMPARTMENT
#include "Atom.h"
#include "Compartment.h"
#include "Instantiation.h"
#include "Term.h"
#include <vector>
#include <string>

class OCompartment : public Compartment {
	void merge_vectors(const std::vector<Term *> &, std::vector<Term *> &) const;
public:
	const std::vector<std::string *> &wrap_variables_symbols;
	const std::vector<std::string *> &content_variables_symbols;

	OCompartment(std::vector<Term *> &, std::vector<const Term *> &, const std::vector<std::string *> &, const std::vector<std::string *> &);
	~OCompartment();

	Compartment * ground(instantiationType &) const;
	virtual void update_add(const Term &) const;

protected:
	virtual std::ostream& write(std::ostream &) const;
};
#endif
