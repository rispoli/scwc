#ifndef TERM
#define TERM
#include <inttypes.h>
#include <ostream>
#include <typeinfo>

class SubMatchSet;
typedef int64_t multiplicityType;

class Term {
public:
	virtual ~Term() {}
	friend std::ostream& operator<<(std::ostream &, const Term &);
	bool operator==(const Term &);
	bool operator!=(const Term &);
	bool operator<=(const Term &);
	bool operator>(const Term &);

	// Returns -1 for match not found
	virtual multiplicityType match(const Term &, SubMatchSet *) const = 0;
	virtual multiplicityType update_delete(const Term &) = 0;
	virtual void update_add(const Term &) = 0;

protected:
	virtual std::ostream& write(std::ostream &) const = 0;
	virtual bool equals(const Term &) const = 0;
	virtual bool leq(const Term &) const = 0;
};

inline std::ostream& operator<<(std::ostream &os, const Term &t) { return t.write(os); }
inline bool Term::operator==(const Term &o) { if(typeid(*this) != typeid(o)) return false; return equals(o); }
inline bool Term::operator!=(const Term &o) { return !(*this == o); }
// Works because typeid(Atom).name() < typeid(Compartment).name():
inline bool Term::operator<=(const Term &o) { if(typeid(*this) == typeid(o)) return leq(o); return typeid(*this).name() < typeid(o).name(); }
inline bool Term::operator>(const Term &o) { return !(*this <= o); }
#endif
