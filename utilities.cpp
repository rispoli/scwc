#include "utilities.h"

int vfind(const std::vector<Term *> &v, const Term *p) {
	for(unsigned int i = 0; i < v.size(); ++i)
		if(v[i] == p) return i;
	return -1;
}
