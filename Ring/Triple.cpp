#include "Triple.h"

Triple::Triple() {

}

Triple::Triple(Term* s, Term* p, Term* o) {
    this->s = s;
    this->p = p;
    this->o = o;
    this->s_score = -2;
    this->p_score = -2;
    this->o_score = -2;
}

void Triple::set_scores(vector<string>& gao) {
    int count = 0;
    if (!this->s->isVariable) {
        this->s_score = -1;
        count++;
    }
    if (!this->p->isVariable) {
        this->p_score = -1;
        count++;
    }
    if (!this->o->isVariable) {
        this->o_score = -1;
        count++;
    }
    int prev_count = count - 1;
    for (string current_var : gao) {
        if (this->s->isVariable && this->s->varname.compare(current_var) == 0) {
            this->s_score = count;
            count++;
        } else if (this->p->isVariable && this->p->varname.compare(current_var) == 0) {
            this->p_score = count;
            count++;
        } else if (this->o->isVariable && this->o->varname.compare(current_var) == 0) {
            this->o_score = count;
            count++;
        }
        if (count >= 3) {
            break;
        }
    }
    if (this->s->isVariable) {
        this->s_score -= prev_count;
    }
    if (this->p->isVariable) {
        this->p_score -= prev_count;
    }
    if (this->o->isVariable) {
        this->o_score -= prev_count;
    }


}

void Triple::serialize() {
    if (this->s->isVariable) {
        std::cout << "s: " << this->s->varname << endl;
    } else {
        std::cout << "s: " << this->s->constant << endl;
    }

    if (this->p->isVariable) {
        std::cout << "p: " << this->p->varname << endl;
    } else {
        std::cout << "p: " << this->p->constant << endl;
    }

    if (this->o->isVariable) {
        std::cout << "o: " << this->o->varname << endl;
    } else {
        std::cout << "o: " << this->o->constant << endl;
    }

    std::cout << "score s: " << this->s_score << endl;
    std::cout << "score p: " << this->p_score << endl;
    std::cout << "score o: " << this->o_score << endl;

}

void Triple::serialize_as_triple_pattern() {
    if (this->s->isVariable) {
        std::cout << this->s->varname << " ";
    } else {
        std::cout << this->s->constant << " ";
    }

    if (this->p->isVariable) {
        std::cout << this->p->varname << " ";
    } else {
        std::cout << this->p->constant << " ";
    }

    if (this->o->isVariable) {
        std::cout << this->o->varname << " ";
    } else {
        std::cout << this->o->constant << " ";
    }

    std::cout << " . " << endl;
}

bool Triple::contains_variable(string varname) {
    if (this->s->isVariable) {
        if (this->s->varname.compare(varname) == 0) {
            return true;
        }
    } 
    
    if (this->p->isVariable) {
        if (this->p->varname.compare(varname) == 0) {
            return true;
        }
    }

    if (this->o->isVariable) {
        if (this->o->varname.compare(varname) == 0) {
            return true;
        }
    }

    return false;
}