#include "PCompartment.h"

PCompartment::PCompartment(std::vector<Term *> &w, std::vector<const Term *> &c, const std::string &wv, const std::string &cv) : Compartment(w, (std::vector<Term *> &)c), wrap_variable_symbol(wv), content_variable_symbol(cv) {}

PCompartment::~PCompartment() {
	delete &wrap_variable_symbol;
	delete &content_variable_symbol;
}

multiplicityType PCompartment::match(const Term &t, SubMatchSet *sms) const {
	if(typeid(t) == typeid(Atom)) return -1;

	const Compartment &c = static_cast<const Compartment &>(t);

	if(wrap.size() > c.wrap.size() || content.size() > c.content.size()) return -1;

	multiplicityType pcr = 1;
	unsigned int i = 0, j = 0;
	while(i < wrap.size() && j < c.wrap.size()) {
		multiplicityType r = wrap[i]->match(*c.wrap[j], NULL);
		if(r != -1) { pcr *= r; ++i; }
		++j;
	}
	if(i != wrap.size()) return -1;

	i = 0; j = 0;
	while(i < content.size() && j < c.content.size()) {
		multiplicityType r;
		if(typeid(*content[i]) == typeid(PCompartment) && typeid(*c.content[j]) == typeid(Compartment)) {
			multiplicityType sub_r = 1;
			for(unsigned int k = 0; k < c.content.size(); ++k) {
				if(typeid(*c.content[k]) == typeid(Compartment)) {
					multiplicityType r_k = sms->submatch_rule_term(NULL, *content[i], c.content, *c.content[k], NULL, NULL);
					if(r_k != -1) { sub_r *= r_k; }
				}
			}
			r = sub_r;
		} else
			r = content[i]->match(*c.content[j], NULL);
		if(r != -1) { pcr *= r; ++i; }
		++j;
	}
	if(i != content.size()) return -1;

	return pcr;
}

std::ostream& PCompartment::write(std::ostream &os) const {
	os << "( ";
	for(unsigned int i = 0; i < wrap.size(); ++i)
		os << *wrap[i] << " ";
	os << wrap_variable_symbol << " | ";
	for(unsigned int i = 0; i < content.size(); ++i)
		os << *content[i] << " ";
	return os << content_variable_symbol << " )";
}
