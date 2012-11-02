#include "OCompartment.h"

void OCompartment::merge_vectors(const std::vector<Term *> &osrc, std::vector<Term *> &dst) const {
	std::vector<Term *> src;
	for(unsigned int i = 0; i < osrc.size(); ++i)
		if(typeid(*osrc[i]) == typeid(Atom))
			src.push_back(new Atom(*static_cast<Atom *>(osrc[i])));
		else if(typeid(*osrc[i]) == typeid(Compartment))
			src.push_back(new Compartment(*static_cast<Compartment *>(osrc[i])));
	unsigned int i = 0, j = 0;
	while(i < src.size()) {
		if(j < dst.size())
			if(*(src[i]) == *(dst[j])) {
				dst[j]->update_add(*src[i]);
				delete src[i];
				++j; ++i;
			} else {
				if(*(src[i]) <= *(dst[j])) {
					dst.insert(dst.begin() + j, src[i]);
					++j; ++i;
				} else
					++j;
			}
		else {
			dst.push_back(src[i]);
			++i;
		}
	}
}

OCompartment::OCompartment(std::vector<Term *> &w, std::vector<const Term *> &c, const std::vector<std::string *> &wv, const std::vector<std::string *> &cv) : Compartment(w, (std::vector<Term *> &)c), wrap_variables_symbols(wv), content_variables_symbols(cv) {}

OCompartment::~OCompartment() {
	for(unsigned int i = 0; i < wrap_variables_symbols.size(); ++i)
		delete wrap_variables_symbols[i];
	delete &wrap_variables_symbols;
	for(unsigned int i = 0; i < content_variables_symbols.size(); ++i)
		delete content_variables_symbols[i];
	delete &content_variables_symbols;
}

Compartment * OCompartment::ground(instantiationType &i) const {
	Compartment *c = new Compartment(*this);
	for(unsigned int j = 0; j < wrap_variables_symbols.size(); ++j) {
		instantiationType::const_iterator it = i.find(*wrap_variables_symbols[j]);
		if(it != i.end())
			merge_vectors(*it->second, c->wrap);
	}
	for(unsigned int j = 0; j < content.size(); ++j)
		if(typeid(*content[j]) == typeid(OCompartment)) {
			Compartment *c_j = static_cast<OCompartment *>(content[j])->ground(i);
			delete c->content[j];
			c->content.erase(c->content.begin() + j);
			c->content.insert(c->content.begin() + j, c_j);
		}
	for(unsigned int j = 0; j < content_variables_symbols.size(); ++j) {
		instantiationType::const_iterator it = i.find(*content_variables_symbols[j]);
		if(it != i.end())
			merge_vectors(*it->second, c->content);
	}
	return c;
}

void OCompartment::update_add(const Term &t) const {
	const Compartment &c = static_cast<const Compartment &>(t);
	merge_vectors(wrap, c.wrap);
	merge_vectors(content, c.content);
}

std::ostream& OCompartment::write(std::ostream &os) const {
	os << "( ";
	for(unsigned int i = 0; i < wrap.size(); ++i)
		os << *wrap[i] << " ";
	for(unsigned int i = 0; i < wrap_variables_symbols.size(); ++i)
		os << *wrap_variables_symbols[i] << " ";
	os << "| ";
	for(unsigned int i = 0; i < content.size(); ++i)
		os << *content[i] << " ";
	for(unsigned int i = 0; i < content_variables_symbols.size(); ++i)
		os << *content_variables_symbols[i] << " ";
	return os << ")";
}
