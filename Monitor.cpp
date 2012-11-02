#include "Monitor.h"

Monitor::Monitor(const std::string &t, const std::vector<const Term *> &p) : title(t), pattern(p) {}

Monitor::~Monitor() {
	delete &title;
	for(unsigned int i = 0; i < pattern.size(); ++i)
		delete pattern[i];
	delete &pattern;
}

std::ostream& operator<<(std::ostream &os, const Monitor &m) {
	os << "%m: \"" << m.title << "\": ";
	for(unsigned int i = 0; i < m.pattern.size(); ++i)
		os << *m.pattern[i] << " ";
	return os;
}
