#ifndef COMP_F 
#define COMP_F
#include "Term.h"

class compare_class {
public:
	bool operator()(Term *t1, Term *t2) { return *t1 <= *t2; }
};
#endif
