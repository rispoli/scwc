%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include "Atom.h"
#include "Compartment.h"
#include "Model.h"
#include "Monitor.h"
#include "OCompartment.h"
#include "PCompartment.h"
#include "Rule.h"
#include "Term.h"

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.4.1 or later */
%require "2.4.1"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="scwc"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages */
%error-verbose

/*** Change the grammar's tokens below ***/

%union {
    int64_t                            multiplicity;
    double                             constantRate;
    std::string                        *atomVal;
    std::string                        *qstring;
    std::string                        *termVariable;
    std::vector<std::string *>         *termVariablesMultiset;
    std::string                        *wrapVariable;
    std::vector<std::string *>         *wrapVariablesMultiset;
    class Atom                         *atomNode;
    std::vector<class Term *>          *atomsMultiset;
    class Compartment                  *compartmentNode;
    class Term                         *termNode;
    std::vector<class Term *>          *termsMultiset;
    class PCompartment                 *pcompartmentNode;
    const class Term                   *patternNode;
    std::vector<const class Term *>    *patternsMultiset;
    class OCompartment                 *ocompartmentNode;
    const class Term                   *opentermNode;
    std::vector<const class Term *>    *opentermMultiset;
    const class Rule                   *ruleNode;
    std::vector<const class Rule *>    *rulesList; 
    const class Monitor                *monitorNode;
    std::vector<const class Monitor *> *monitorsList;
    class Model                        *modelNode;
}

%token                  END         0 "end of file"
//%token                  EOL         "end of line"
%token                  RSEP        ">>>"
%token                  LSEP        "%%"
%token                  MODEL_      "%MODEL:"
%token                  RULES       "%rules"
%token                  TERM_       "%term"
%token                  MONITORS    "%monitors"
%token                  MONITOR_    "%m"
%token                  ENDF        "%end"
%token <multiplicity>   INTEGER     "integer"
%token <constantRate>   DOUBLE      "double"
%token <atomVal>        STRING      "string"
%token <qstring>        QSTRING     "qstring"
%token <termVariable>   TVAR        "tvar"
%token <wrapVariable>   WVAR        "wvar"

%type <atomNode>              atom
%type <atomsMultiset>         atoms_multiset
%type <compartmentNode>       compartment
%type <termNode>              term
%type <termsMultiset>         terms_multiset
%type <pcompartmentNode>      pcompartment
%type <patternNode>           pattern
%type <patternsMultiset>      patterns_multiset
%type <termVariablesMultiset> tvar_multiset
%type <wrapVariablesMultiset> wvar_multiset
%type <ocompartmentNode>      ocompartment
%type <opentermNode>          openterm
%type <opentermMultiset>      openterm_multiset
%type <ruleNode>              rule
%type <rulesList>             rules_list
%type <monitorNode>           monitor
%type <monitorsList>          monitors_list
%type <modelNode>             model

/*** Change the grammar's tokens above ***/

%{

#include "Driver.h"
#include "scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

/*** Change the grammar rules below ***/

atom : STRING { $$ = new Atom(*new std::string(*$1)); delete $1; }
     | INTEGER '*' STRING { if($1 <= 0) {
                                error(yyloc, std::string("Multiplicity for atom \"") + *$3 + std::string("\" must be > 0"));
                                delete $3;
                                YYERROR;
                            } else {
                                $$ = new Atom(*new std::string(*$3), $1);
                                delete $3;
                            }}

atoms_multiset : atoms_multiset atom { $1->push_back($2); $$ = $1; }
               | /* empty */ { $$ = new std::vector<class Term *>; }

compartment : '(' atoms_multiset '|' terms_multiset ')' { $$ = new Compartment(*$2, *$4); }

term : atom { $$ = $1; }
     | compartment { $$ = $1; }

terms_multiset : terms_multiset term { $1->push_back($2); $$ = $1; }
               | /* empty */ { $$ = new std::vector<class Term *>; }

pcompartment : '(' atoms_multiset WVAR '|' patterns_multiset TVAR ')' { $$ = new PCompartment(*$2, *$5, *$3, *$6); }
             | '(' atoms_multiset WVAR '|' TVAR ')' { $$ = new PCompartment(*$2, *new std::vector<const Term *>, *$3, *$5); }

pattern : term { $$ = $1; }
        | pcompartment { $$ = $1; }

patterns_multiset : patterns_multiset pattern { $1->push_back($2); $$ = $1; }
                  | pattern { std::vector<const class Term *> *v = new std::vector<const class Term *>; v->push_back($1); $$ = v; }

tvar_multiset : tvar_multiset TVAR { $1->push_back($2); $$ = $1; }
              | /* empty */ { $$ = new std::vector<std::string *>; }

wvar_multiset : wvar_multiset WVAR { $1->push_back($2); $$ = $1; }
              | /* empty */ { $$ = new std::vector<std::string *>; }

ocompartment : '(' atoms_multiset wvar_multiset '|' openterm_multiset tvar_multiset ')' { $$ = new OCompartment(*$2, *$5, *$3, *$6); }

openterm : atom { $$ = $1; }
         | ocompartment { $$ = $1; }

openterm_multiset : openterm_multiset openterm { $1->push_back($2); $$ = $1; }
                  | /* empty */ { $$ = new std::vector<const class Term *>; }

rule : patterns_multiset TVAR RSEP '[' DOUBLE ']' RSEP openterm_multiset tvar_multiset { int p_lhs = 0;
                                                                                         for(unsigned int i = 0; i < $1->size(); ++i)
                                                                                             if(typeid(*(*$1)[i]) == typeid(PCompartment)) ++p_lhs;
                                                                                         if(p_lhs > 2) {
                                                                                             error(yyloc, std::string("No more than two compartment for rule are allowed"));
                                                                                             for(unsigned int i = 0; i < $1->size(); ++i) delete (*$1)[i];
                                                                                             delete $1;
                                                                                             for(unsigned int i = 0; i < $8->size(); ++i) delete (*$8)[i];
                                                                                             delete $8;
                                                                                             YYERROR;
                                                                                         } else {
                                                                                             bool f = false;
                                                                                             if(p_lhs == 2) {
                                                                                                 f = true;
                                                                                                 error(yyloc, std::string("Warning: two compartments in the left hand side of a rule, possible danger (Hic sunt monstra!)"));
                                                                                             }
                                                                                             $$ = new Rule(*$1, *$8, $5, *$2, *$9, f); } }

rules_list : rules_list rule LSEP { $1->push_back($2); $$ = $1; }
           | rule LSEP { std::vector<const class Rule *> *v = new std::vector<const class Rule *>; v->push_back($1); $$ = v; }

monitor : MONITOR_ QSTRING ':' patterns_multiset { std::string *t = new std::string($2->substr(1, $2->size() - 2)); $$ = new Monitor(*t, *$4); delete $2; }

monitors_list : monitors_list monitor LSEP { $1->push_back($2); $$ = $1; }
              | monitor LSEP { std::vector<const class Monitor *> *v = new std::vector<const class Monitor *>; v->push_back($1); $$ = v; }

model : MODEL_ QSTRING RULES rules_list TERM_ terms_multiset MONITORS monitors_list ENDF { std::string *t = new std::string($2->substr(1, $2->size() - 2)); $$ = new Model(*t, *$4, *$6, *$8); delete $2; }

start : /* empty */
      | start model END { driver.model = $2; }

/*** Change the grammar rules above ***/

%% /*** Additional Code ***/

void scwc::Parser::error(const Parser::location_type& l,
                const std::string& m)
{
    driver.error(l, m);
}
