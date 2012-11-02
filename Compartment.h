#ifndef COMPARTMENT
#define COMPARTMENT
#include "Atom.h"
#include "Comparison.h"
#include "Term.h"
#include <algorithm>
#include <vector>
#include <typeinfo>

class PCompartment;

class Compartment : public Term {
public:
	std::vector<Term *> &wrap;
	std::vector<Term *> &content;

	Compartment(std::vector<Term *> &, std::vector<Term *> &);
	Compartment(const Compartment &copy);
	~Compartment();

	virtual multiplicityType match(const Term &, SubMatchSet *) const;
	virtual multiplicityType update_delete(const Term &);
	virtual void update_add(const Term &);

protected:
	virtual std::ostream& write(std::ostream &) const;
	virtual bool equals(const Term &) const;
	virtual bool leq(const Term &) const;
};
#endif
