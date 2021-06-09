#ifndef LEAPFROG_TERM_H
#define LEAPFROG_TERM_H
#include <string>
#include <iostream>
using namespace std;



class Term {
    public:
        bool isVariable;
        string varname;
        uint64_t constant;
        Term();
        Term(uint64_t);
        Term(string);
};


#endif //LEAPFROG_TERM_H
