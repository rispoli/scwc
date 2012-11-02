#include "Atom.h"
#include "Compartment.h"
#include "Instantiation.h"
#include "Match.h"
#include "MatchSet.h"
#include "OCompartment.h"
#include "PCompartment.h"
#include "randomc/randomc.h"
#include "randomc/sfmt.h"
#include "Rule.h"
#include "SubMatch.h"
#include "SubMatchSet.h"
#include "utilities.h"
#include <cmath>
#include <map>
#include <typeinfo>
#include <utility>

double rn(void);

std::pair<const Rule*, Match *> get_mu(MatchSet &, double);

std::map<const Term *, SubMatch *> * get_updates(SubMatchSet &);

void update_or_insert(const Term &, std::vector<Term *> &);

std::vector<Term *> * get_copy(std::vector<Term *> &);

void insert_in_order(std::vector<Term *> &, Term &);

void copy_in_term(std::vector<Term *> &, std::vector<Term *> &);

void do_updates(const Rule &, std::map<const Term *, SubMatch *> &);

void gillespie(MatchSet &, double &, CRandomSFMT0 *);
