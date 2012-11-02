#include "Atom.h"

multiplicityType Atom::binomial(const multiplicityType n, const multiplicityType k) const {
	//http://blog.plover.com/math/choose.html
	if(k > n) return 0;
	if(k == 1) return n;
	multiplicityType b = 1;
	multiplicityType _n = n;
	for(multiplicityType d = 1; d <= k; ++d) {
		b *= _n--;
		b /= d;
	}
	return b;
}

Atom::Atom(const std::string &a, multiplicityType m) : atom(a), multiplicity(m) {}

Atom::Atom(const Atom &copy) : atom(*new std::string(copy.atom)), multiplicity(copy.multiplicity) {}

Atom::~Atom() {
	delete &atom;
}

multiplicityType Atom::match(const Term &t, SubMatchSet *) const {
	if(typeid(t) != typeid(Atom)) return -1;
	const Atom &a = static_cast<const Atom &>(t);
	multiplicityType r = -1;
	if(atom == a.atom && multiplicity <= a.multiplicity)
		r = binomial(a.multiplicity, multiplicity); // pay attention to the order of the parameters in the call
	return r;
}

multiplicityType Atom::update_delete(const Term &t) {
	multiplicity -= static_cast<const Atom &>(t).multiplicity;
	return multiplicity;
}

void Atom::update_add(const Term &t) {
	multiplicity += static_cast<const Atom &>(t).multiplicity;
}

std::ostream& Atom::write(std::ostream &os) const {
	return os << multiplicity << "*" << atom;
}

bool Atom::equals(const Term &t) const {
	const Atom &a = static_cast<const Atom &>(t);
	return atom == a.atom;
}

bool Atom::leq(const Term &t) const {
	const Atom &a = static_cast<const Atom &>(t);
	return atom <= a.atom;
}
