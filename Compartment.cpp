#include "Compartment.h"
#include "PCompartment.h"

Compartment::Compartment(std::vector<Term *> &w, std::vector<Term *> &c) : wrap(w), content(c) {
	compare_class compare_function_obj;
	sort(wrap.begin(), wrap.end(), compare_function_obj);
	sort(content.begin(), content.end(), compare_function_obj);
}

Compartment::Compartment(const Compartment &copy) : wrap(*new std::vector<Term *>), content(*new std::vector<Term *>) {
	for(unsigned int i = 0; i < copy.wrap.size(); ++i)
		wrap.push_back(new Atom((Atom &)*copy.wrap[i]));
	for(unsigned int i = 0; i < copy.content.size(); ++i)
		if(typeid(*copy.content[i]) == typeid(Atom))
			content.push_back(new Atom((Atom &)*copy.content[i]));
		else
			content.push_back(new Compartment((Compartment &)*copy.content[i]));
}

Compartment::~Compartment() {
	for(unsigned int i = 0; i < wrap.size(); ++i)
		delete wrap[i];
	delete &wrap;
	for(unsigned int i = 0; i < content.size(); ++i)
		delete content[i];
	delete &content;
}

multiplicityType Compartment::match(const Term &t, SubMatchSet *) const {
	if(typeid(t) != typeid(Compartment)) return -1;
	const Compartment &c = static_cast<const Compartment &>(t);
	multiplicityType cr = 1;
	if(wrap.size() != c.wrap.size() || content.size() != c.content.size()) return -1;
	for(unsigned int i = 0; i < wrap.size(); ++i) {
		multiplicityType r = wrap[i]->match(*c.wrap[i], NULL);
		if(r == -1) return -1;
		else cr *= r;
	}
	for(unsigned int i = 0; i < content.size(); ++i) {
		multiplicityType r = content[i]->match(*c.content[i], NULL);
		if(r == -1) return -1;
		else cr *= r;
	}
	return cr;
}

multiplicityType Compartment::update_delete(const Term &t) {
	const Compartment &c = static_cast<const Compartment &>(t);
	multiplicityType r = 0;
	for(unsigned int i = 0; i < c.wrap.size(); ++i)
		for(unsigned int j = 0; j < wrap.size(); ++j)
			if(*c.wrap[i] == *wrap[j]) {
				multiplicityType tr = 0;
				tr = wrap[j]->update_delete(*c.wrap[i]);
				if(tr == 0) {
					delete wrap[j];
					wrap.erase(wrap.begin() + j);
				}
				r += tr;
				break;
			}
	for(unsigned int i = 0; i < c.content.size(); ++i)
		for(unsigned int j = 0; j < content.size(); ++j) {
			if(typeid(*c.content[i]) != typeid(PCompartment) && *c.content[i] == *content[j]) {
				multiplicityType tr = 0;
				tr = content[j]->update_delete(*c.content[i]);
				if(tr == 0) {
					delete content[j];
					content.erase(content.begin() + j);
				}
				r += tr;
				break;
			}
		}
	return r;
}

void Compartment::update_add(const Term &t) {
	const Compartment &c = static_cast<const Compartment &>(t);
	for(unsigned int i = 0; i < wrap.size(); ++i) wrap[i]->update_add(*c.wrap[i]);
	for(unsigned int i = 0; i < content.size(); ++i) content[i]->update_add(*c.content[i]);
}

std::ostream& Compartment::write(std::ostream &os) const {
	os << "( ";
	for(unsigned int i = 0; i < wrap.size(); ++i)
		os << *wrap[i] << " ";
	os << "| ";
	for(unsigned int i = 0; i < content.size(); ++i)
		os << *content[i] << " ";
	return os << ")";
}

bool Compartment::equals(const Term &t) const {
	const Compartment &c = static_cast<const Compartment &>(t);
	if(wrap.size() != c.wrap.size() || content.size() != c.content.size()) return false;
	for(unsigned int i = 0; i < wrap.size(); ++i) if(*wrap[i] != *c.wrap[i]) return false;
	for(unsigned int i = 0; i < content.size(); ++i) if(*content[i] != *c.content[i]) return false;
	return true;
}

bool Compartment::leq(const Term &t) const {
	const Compartment &c = static_cast<const Compartment &>(t);
	if(wrap.size() != c.wrap.size() || content.size() != c.content.size()) return wrap.size() <= c.wrap.size() && content.size() <= c.content.size();
	for(unsigned int i = 0; i < wrap.size(); ++i) if(*wrap[i] > *c.wrap[i]) return false;
	for(unsigned int i = 0; i < content.size(); ++i) if(*content[i] > *c.content[i]) return false;
	return true;
}
