#ifndef ATOM
#define ATOM
#include "Term.h"
#include <string>

class Atom : public Term {
	multiplicityType binomial(const multiplicityType, const multiplicityType) const;
public:
	const std::string &atom;
	multiplicityType multiplicity;

	Atom(const std::string &, multiplicityType = 1);
	Atom(const Atom &copy);
	~Atom();

	virtual multiplicityType match(const Term &, SubMatchSet *) const;
	virtual multiplicityType update_delete(const Term &);
	virtual void update_add(const Term &);

protected:
	virtual std::ostream& write(std::ostream &) const;
	virtual bool equals(const Term &) const;
	virtual bool leq(const Term &) const;
};
#endif
