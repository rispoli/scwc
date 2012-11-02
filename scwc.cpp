#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>

#include "Driver.h"
#include "Gillespie.h"
#include "MatchSet.h"
#include "progress.h"
#include "randomc/randomc.h"
#include "randomc/sfmt.h"
#include "output.h"
using namespace std;

typedef numeric_limits<double> dbl;

int main(int argc, char *argv[]) {

	int seed = (int)time(0);
	CRandomSFMT0 RanGen(seed);

	double time_limit = 0;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "this help message")
		("input-file,i", po::value<string>(), "input file")
		("output-file,o", po::value<string>(), "output file")
		("time-limit,t", po::value<double>(&time_limit)->default_value(100.0), "time limit")
		("sampling-period,s", po::value<double>(), "sampling period")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if(vm.count("help")) {
		cout << desc << endl;
		return 1;
	}

	ifstream *infile = NULL;
	if(vm.count("input-file")) {
		infile = new ifstream(vm["input-file"].as<string>().c_str());
		if(!infile->good()) {
			delete infile;
			cerr << "Could not open file: " << vm["input-file"].as<string>() << endl;
			return 1;
		}
	} else {
		cerr << "No input file provided" << endl;
		return 1;
	}

	ofstream *outfile = NULL;
	if(vm.count("output-file")) {
		outfile = new ofstream(vm["output-file"].as<string>().c_str());
		if(!outfile->is_open()) {
			delete outfile;
			cerr << "Could not open file: " << vm["output-file"].as<string>() << endl;
			return 1;
		}
	} else {
		cerr << "No output file provided" << endl;
		return 1;
	}

	double sampling_period = -1;
	if(vm.count("sampling-period"))
		sampling_period = vm["sampling-period"].as<double>();

	cout.precision(dbl::digits10);

	if(sampling_period > 0) {

		string t, v, last_state = "";
		double time, next_time = 0;

		do {
			getline(*infile, v);
			write_to_file(*outfile, v + "\n");
		} while(infile->good() && v.substr(0, 1) == "#");

		do {
			getline(*infile, t, '\t');
			getline(*infile, v);

			istringstream i(t);
			if((i >> time)) {
				ostringstream o;
				o.precision(dbl::digits10);

				if(next_time < time) {
					o << std::fixed << next_time << "\t" << last_state << "\n";
					write_to_file(*outfile, o.str());
					next_time += sampling_period;
				} else
					last_state = v;
			}
		} while(infile->good());

	} else {

		int percent = 0;

		scwc::Driver driver;

		bool result = driver.parse_stream(*infile, vm["input-file"].as<string>().c_str());
		if(result) {

			double time = 0;

			write_to_file(*outfile, driver.model->header(time_limit));
			write_to_file(*outfile, driver.model->count(time));
			cout << "Simulating: " << driver.model->title << ":" << endl;
			progress_bar(percent);

			MatchSet *matchset = NULL;

			do {

				delete matchset;
				matchset = new MatchSet;
				matchset->match(*driver.model);

				if(matchset->rate_sum != 0) {

					gillespie(*matchset, time, &RanGen);
					write_to_file(*outfile, driver.model->count(time));

				}

				int new_p = (time * 100) / time_limit;
				if(new_p > percent) { percent = new_p; progress_bar(percent); }

			} while(time < time_limit && matchset->rate_sum != 0);

			delete matchset;

			cout << endl;
		}

	}

	outfile->close();
	delete infile;
	delete outfile;

	return 0;
}
