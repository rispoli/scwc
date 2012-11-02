#include "Model.h"

multiplicityType Model::count_(const std::vector<const Term *> &pattern, std::vector<Term *> &term) {

	multiplicityType multiplicity = 1;
	unsigned int counter = 0;
	std::vector<Term *> rec_calls;

	for(unsigned int i = 0; i < pattern.size(); ++i) {
		multiplicityType multiplicity_t = 0;
		for(unsigned int j = 0; j < term.size(); ++j) {
			multiplicityType sub_multiplicity = pattern[i]->match(*term[j], NULL);
			if(sub_multiplicity != -1) multiplicity_t += sub_multiplicity;

			if(typeid(*term[j]) == typeid(Compartment) && vfind(rec_calls, term[j]) == -1)
				rec_calls.push_back(term[j]);

		}
		if(multiplicity_t != 0) { multiplicity *= multiplicity_t; ++counter; }
	}

	if(counter < pattern.size()) { multiplicity = 0; counter = 0; }

	for(unsigned int i = 0; i < rec_calls.size(); ++i) {
		Compartment &c(static_cast<Compartment &>(*rec_calls[i]));
		multiplicityType m1 = count_(pattern, c.wrap);
		if(m1 > 0) { multiplicity += m1; counter += pattern.size(); }
		multiplicityType m2 = count_(pattern, c.content);
		if(m2 > 0) { multiplicity += m2; counter += pattern.size(); }
	}

	return (counter >= pattern.size()) ? multiplicity : 0;
}

Model::Model(const std::string &ti, const std::vector<const Rule *> &r, std::vector<Term *> &t, const std::vector<const Monitor *> &m) : title(ti), rules(r), term(t), monitors(m) {
	compare_class compare_function_obj;
	sort(term.begin(), term.end(), compare_function_obj);
}

Model::~Model() {
	delete &title;
	for(unsigned int i = 0; i < rules.size(); ++i)
		delete rules[i];
	delete &rules;
	for(unsigned int i = 0; i < term.size(); ++i)
		delete term[i];
	delete &term;
	for(unsigned int i = 0; i < monitors.size(); ++i)
		delete monitors[i];
	delete &monitors;
}

std::string Model::header(double time_limit) {
	std::stringstream s;
	s.precision(dbl::digits10);
	s << "# Simulation on model '" << title << "' (end at " << std::fixed << time_limit << ")\n";
	s << "# column 1: Time\n";
	for(unsigned int i = 0; i < monitors.size(); ++i)
		s << "# column " << (i + 2) << ": " << monitors[i]->title << "\n";
	s << "\n";
	return s.str();
}

std::string Model::count(double time) {
	std::stringstream s;
	s.precision(dbl::digits10);
	s << std::fixed << time << "\t";
	for(unsigned int i = 0; i < monitors.size(); ++i)
		s << count_(monitors[i]->pattern, term) << "\t";
	std::string o = s.str();
	return o.erase(o.length() - 1) + "\n";
}

std::ostream& operator<<(std::ostream &os, const Model &m) {
	os << "%MODEL: \"" << m.title << "\"\n\n";
	os << "%rules\n";
	for(unsigned int i = 0; i < m.rules.size(); ++i)
		os << *m.rules[i] << "%%\n";
	os << "\n%term\n";
	for(unsigned int i = 0; i < m.term.size(); ++i)
		os << *m.term[i] << " ";
	os << "\n\n%monitors\n";
	for(unsigned int i = 0; i < m.monitors.size(); ++i)
		os << *m.monitors[i] << "%%\n";
	return os << "\n%end";
}
