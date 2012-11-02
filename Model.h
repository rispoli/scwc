#ifndef MODEL
#define MODEL
#include "Comparison.h"
#include "Compartment.h"
#include "Monitor.h"
#include "Rule.h"
#include "Term.h"
#include "utilities.h"
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>
#include <vector>

typedef std::numeric_limits<double> dbl;

class Model {
	multiplicityType count_(const std::vector<const Term *> &, std::vector<Term *> &);
public:
	const std::string &title;
	const std::vector<const Rule *> &rules;
	std::vector<Term *> &term;
	const std::vector<const Monitor *> &monitors;

	Model(const std::string &, const std::vector<const Rule *> &, std::vector<Term *> &, const std::vector<const Monitor *> &);
	~Model();

	std::string header(double);
	std::string count(double);

	friend std::ostream& operator<<(std::ostream &, const Model &);
};
#endif
