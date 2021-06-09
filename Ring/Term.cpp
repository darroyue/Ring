#include "Term.h"

Term::Term() {

}

Term::Term(uint64_t number) {
    this->constant = number;
    this->isVariable = false;
}

Term::Term(string variable) {
    this->varname = variable;
    this->isVariable = true;
}