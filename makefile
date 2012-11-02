CC=/usr/bin/g++
CFLAGS=-O3 -msse2 -W -Wall -Wextra -Werror -pedantic
LDFLAGS=-lboost_program_options-mt
LEX=/usr/bin/flex
YACC=/usr/bin/bison
SED=/bin/sed
SOURCES:=$(filter-out parser.cpp scanner.cpp, $(wildcard *.cpp))
OBJECTS:=$(filter-out scwc.o, $(SOURCES:.cpp=.o))
BOOST=/usr/include/boost
RANDOM=./randomc
EXECUTABLE=scwc

all: $(EXECUTABLE)

# sed: avoid warning on position.hh@line 136
parser.cpp: parser.yy
	$(YACC) -o parser.cpp --defines=parser.h parser.yy
	@$(SED) -i~ 's,pos1.filename && pos2.filename && \*pos1.filename == \*pos2.filename,(pos1.filename \&\& pos2.filename \&\& *pos1.filename == *pos2.filename),' position.hh

# pedantic works on GNU if you uncomment the isatty (int ) throw(); patch below
scanner.cpp: scanner.ll
	$(LEX) -o scanner.cpp scanner.ll
	@$(SED) -i~ 's,extern "C" int isatty (int );,extern "C" int isatty (int ) throw();,' $@

$(RANDOM)/sfmt.o:
	$(CC) -O3 -msse2 -c $(RANDOM)/sfmt.cpp -o $(RANDOM)/sfmt.o

$(RANDOM)/userintf.o:
	$(CC) -O3 -msse2 -c $(RANDOM)/userintf.cpp -o $(RANDOM)/userintf.o

scwc.o:
	$(CC) $(CFLAGS) -I $(BOOST) -I $(RANDOM) -c scwc.cpp -o scwc.o

$(EXECUTABLE): parser.o scanner.o scwc.o $(RANDOM)/sfmt.o $(RANDOM)/userintf.o $(OBJECTS)
	$(CC) $(LDFLAGS) parser.o scanner.o scwc.o $(RANDOM)/sfmt.o $(RANDOM)/userintf.o $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE) parser.cpp parser.h parser.o position.hh position.hh~ location.hh stack.hh scanner.cpp scanner.cpp~ scanner.o scwc.o

############################################################## Dependencies ##############################################################

Atom.o: Atom.cpp Atom.h Term.h
Comparison.h: Term.h
Compartment.o: Compartment.cpp Compartment.h Atom.h Comparison.h PCompartment.h Term.h
Driver.o: Driver.cpp Driver.h Model.h
Gillespie.o: Gillespie.cpp Gillespie.h Atom.h Compartment.h Instantiation.h Match.h MatchSet.h OCompartment.h PCompartment.h Rule.h SubMatch.h SubMatchSet.h utilities.h
Instantiation.h: Term.h
Match.h: SubMatchSet.h
MatchSet.o: MatchSet.cpp MatchSet.h Match.h Model.h Rule.h Term.h
Model.o: Model.cpp Model.h Comparison.h Compartment.h Monitor.h Rule.h Term.h utilities.h
Monitor.o: Monitor.cpp Monitor.h Term.h
OCompartment.o: OCompartment.cpp OCompartment.h Atom.h Compartment.h Instantiation.h Term.h
parser.o: parser.yy Atom.h Compartment.h Driver.h Model.h Monitor.h OCompartment.h PCompartment.h Rule.h scanner.h Term.h
PCompartment.o: PCompartment.cpp PCompartment.h Atom.h Compartment.h SubMatchSet.h Term.h
Rule.o: Rule.cpp Rule.h Compartment.h OCompartment.h PCompartment.h Term.h
scanner.o: scanner.ll scanner.h
scwc.o: scwc.cpp Driver.h Gillespie.h MatchSet.h progress.h output.h
SubMatch.h: Term.h
SubMatchSet.o: SubMatchSet.cpp SubMatchSet.h Compartment.h MatchSet.h Rule.h SubMatch.h Term.h utilities.h
utilities.o: utilities.cpp utilities.h Term.h

##########################################################################################################################################
