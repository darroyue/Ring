#ifndef LEAPFROG_TRIPLE_H
#define LEAPFROG_TRIPLE_H
#include "Term.h"
#include <vector>

class Triple {
    public:
        Triple();
        Triple(Term* s, Term* p, Term* o);
        
        Term* s;
        Term* p;
        Term* o;
        int s_score;
        int p_score;
        int o_score;
        void set_scores(vector<string>&);
        void serialize();
        void serialize_as_triple_pattern();
        bool contains_variable(string);
};


#endif //LEAPFROG_TRIPLE_H
