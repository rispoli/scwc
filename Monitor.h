#ifndef MONITOR
#define MONITOR
#include "Term.h"
#include <string>
#include <vector>

class Monitor {
public:
	const std::string &title;
	const std::vector<const Term *> &pattern;

	Monitor(const std::string &, const std::vector<const Term *> &);
	~Monitor();

	friend std::ostream& operator<<(std::ostream &, const Monitor &);
};
#endif
