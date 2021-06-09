#ifndef LEAPFROG_ITERATOR_H
#define LEAPFROG_ITERATOR_H

#include <iostream>
#include "Triple.h"
#include "triple_bwt.hpp"

class Iterator {
public:
    Triple* triple;
    triple_bwt* graph;
    string index_name;
    int current_level;
    bwt_interval i_s;
    bwt_interval i_p;
    bwt_interval i_o;
    uint64_t cur_s;
    uint64_t cur_p;
    uint64_t cur_o;
    bool is_empty;
    
    Iterator(Triple* triple, triple_bwt* graph) {
        this->triple = triple;
        this->graph = graph;
        this->current_level = -1;
        this->is_empty = false;

        // SPO
        if (triple->s_score == -1 && triple->p_score == -1) {
            this->index_name = "SPO";
            
            // Down from before-first to first
            this->i_s = this->graph->open_SPO();
            this->current_level += 1;

            this->cur_s = this->graph->min_S(this->i_s);
            this->cur_s = this->graph->next_S(this->i_s, this->triple->s->constant);

            // Down
            this->i_p = this->graph->down_S(this->cur_s);
            this->current_level += 1;

            this->cur_p = this->graph->min_P_in_S(this->i_p, this->cur_s);
            this->cur_p = this->graph->next_P_in_S(this->i_p, this->cur_s, this->triple->p->constant);

            if (this->cur_s == 0 || this->cur_p == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == -1 && triple->p_score == 1 && triple->o_score == 2) {
            this->index_name = "SPO";
            
            // Down from before-first to first
            this->i_s = this->graph->open_SPO();
            this->current_level += 1;

            this->cur_s = this->graph->min_S(this->i_s);
            this->cur_s = this->graph->next_S(this->i_s, this->triple->s->constant);

            if (this->cur_s == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 1 && triple->p_score == 2 && triple->o_score == 3) {
            this->index_name = "SPO";

            // Ready to perform down()
        }

        // SOP
        else if (triple->s_score == -1 && triple->o_score == -1) {
            this->index_name = "SOP";
            
            // Down from before-first to first
            this->i_s = this->graph->open_SOP();
            this->current_level += 1;

            this->cur_s = this->graph->min_S(this->i_s);
            this->cur_s = this->graph->next_S(this->i_s, this->triple->s->constant);

            // Down
            this->i_o = this->graph->down_S(this->cur_s);
            this->current_level += 1;

            this->cur_o = this->graph->min_O_in_S(this->i_o);
            this->cur_o = this->graph->next_O_in_S(this->i_o, this->triple->o->constant);

            if (this->cur_s == 0 || this->cur_o == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == -1 && triple->p_score == 2 && triple->o_score == 1) {
            this->index_name = "SOP";
            
            // Down from before-first to first
            this->i_s = this->graph->open_SOP();
            this->current_level += 1;

            this->cur_s = this->graph->min_S(this->i_s);
            this->cur_s = this->graph->next_S(this->i_s, this->triple->s->constant);

            if (this->cur_s == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 1 && triple->p_score == 3 && triple->o_score == 2) {
            this->index_name = "SOP";

            // Ready to perform down()
        }

        // POS
        else if (triple->p_score == -1 && triple->o_score == -1) {
            this->index_name = "POS";
            
            // Down from before-first to first
            this->i_p = this->graph->open_POS();
            this->current_level += 1;

            this->cur_p = this->graph->min_P(this->i_p);
            this->cur_p = this->graph->next_P(this->i_p, this->triple->p->constant);

            // Down
            this->i_o = this->graph->down_P(this->cur_p);
            this->current_level += 1;

            this->cur_o = this->graph->min_O_in_P(this->i_o, this->cur_p);
            this->cur_o = this->graph->next_O_in_P(this->i_o, this->cur_p, this->triple->o->constant);

            if (this->cur_p == 0 || this-> cur_o == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 2 && triple->p_score == -1 && triple->o_score == 1) {
            this->index_name = "POS";
            
            // Down from before-first to first
            this->i_p = this->graph->open_POS();
            this->current_level += 1;

            this->cur_p = this->graph->min_P(this->i_p);
            this->cur_p = this->graph->next_P(this->i_p, this->triple->p->constant);

            if (this->cur_p == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 3 && triple->p_score == 1 && triple->o_score == 2) {
            this->index_name = "POS";

            // Ready to perform down()
        }

        // PSO
        else if (triple->s_score == 1 && triple->p_score == -1 && triple->o_score == 2) {
            this->index_name = "PSO";
            
            // Down from before-first to first
            this->i_p = this->graph->open_PSO();
            this->current_level += 1;

            this->cur_p = this->graph->min_P(this->i_p);
            this->cur_p = this->graph->next_P(this->i_p, this->triple->p->constant);

            if (this->cur_p == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 2 && triple->p_score == 1 && triple->o_score == 3) {
            this->index_name = "PSO";

            // Ready to perform down()
        }

        // OSP
        else if (triple->s_score == 1 && triple->p_score == 2 && triple->o_score == -1) {
            this->index_name = "OSP";
            
            // Down from before-first to first
            this->i_o = this->graph->open_OSP();
            this->current_level += 1;

            this->cur_o = this->graph->min_O(this->i_o);
            this->cur_o = this->graph->next_O(this->i_o, this->triple->o->constant);

            if (this->cur_o == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 2 && triple->p_score == 3 && triple->o_score == 1) {
            this->index_name = "OSP";

            // Ready to perform down()
        }

        // OPS
        else if (triple->s_score == 2 && triple->p_score == 1 && triple->o_score == -1) {
            this->index_name = "OPS";
            
            // Down from before-first to first
            this->i_o = this->graph->open_OPS();
            this->current_level += 1;

            this->cur_o = this->graph->min_O(this->i_o);
            this->cur_o = this->graph->next_O(this->i_o, this->triple->o->constant);

            if (this->cur_o == 0) {
                this->is_empty = true;
            }

        } else if (triple->s_score == 3 && triple->p_score == 2 && triple->o_score == 1) {
            this->index_name = "OPS";

            // Ready to perform down()
        }

    }

    void down() {
        // SPO
        if (this->index_name.compare("SPO") == 0) {
            if (this->current_level == -1) {
                // Down from before-first to first
                this->current_level += 1;
                this->i_s = this->graph->open_SPO();
                this->cur_s = this->graph->min_S(this->i_s);
            } else if (this->current_level == 0) {
                this->current_level += 1;
                this->i_p = this->graph->down_S(this->cur_s);
                this->cur_p = this->graph->min_P_in_S(this->i_p, this->cur_s);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_o = this->graph->down_S_P(this->i_p, this->cur_s, this->cur_p);
                this->cur_o = this->graph->min_O_in_SP(this->i_o);
            }
        }

        // SOP
        else if (this->index_name.compare("SOP") == 0) {
            if (this->current_level == -1) {
                this->current_level += 1;
                this->i_s = this->graph->open_SOP();
                this->cur_s = this->graph->min_S(this->i_s);
            } else if (this->current_level == 0) {
                this->current_level += 1;
                this->i_o = this->graph->down_S(this->cur_s);
                this->cur_o = this->graph->min_O_in_S(this->i_o);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_p = this->graph->down_S_O(this->i_o, this->cur_o);
                this->cur_o = this->graph->min_P_in_SO(this->i_p);
            }
        }

        // POS
        else if (this->index_name.compare("POS") == 0) {
            if (this->current_level == -1) {
                this->current_level += 1;
                this->i_p = this->graph->open_POS();
                this->cur_p = this->graph->min_P(this->i_p);
            } else if (this->current_level == 0) {
                this->current_level += 1;
                this->i_o = this->graph->down_P(this->cur_p);
                this->cur_o = this->graph->min_O_in_P(this->i_o, this->cur_p);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_s = this->graph->down_P_O(this->i_o, this->cur_p, this->cur_o);
                this->cur_s = this->graph->min_S_in_PO(this->i_s);
            }
        }

        // PSO
        else if (this->index_name.compare("PSO") == 0) {
            if (this->current_level == -1) {
                this->current_level += 1;
                this->i_p = this->graph->open_PSO();
                this->cur_p = this->graph->min_P(this->i_p);
            } else if (this->current_level == 0) {
                this->current_level += 1;
                this->i_s = this->graph->down_P(this->cur_p);
                this->cur_s = this->graph->min_S_in_P(this->i_s);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_o = this->graph->down_P_S(this->i_s, this->cur_s);
                this->cur_o = this->graph->min_O_in_PS(this->i_o);
            }
        }

        // OSP
        else if (this->index_name.compare("OSP") == 0) {
            if (this->current_level == -1) {
                this->current_level += 1;
                this->i_o = this->graph->open_OSP();
                this->cur_o = this->graph->min_O(this->i_o);
            } else if (this->current_level == 0) {
                this->current_level += 1;
                this->i_s = this->graph->down_O(this->cur_o);
                this->cur_s = this->graph->min_S_in_O(this->i_s, this->cur_o);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_p = this->graph->down_O_S(this->i_s, this->cur_o, this->cur_s);
                this->cur_p = this->graph->min_P_in_OS(this->i_p);
            }
        }

        // OPS
        else if (this->index_name.compare("OPS") == 0) {
            if (this->current_level == -1) {
                this->current_level += 1;
                this->i_o = this->graph->open_OPS();
                this->cur_o = this->graph->min_O(this->i_o);
            } else if (this->current_level == 0) {
                this->current_level += 1;
                this->i_p = this->graph->down_O(this->cur_o);
                this->cur_p = this->graph->min_P_in_O(this->i_p);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_s = this->graph->down_O_P(this->i_p, this->cur_p);
                this->cur_s = this->graph->min_S_in_OP(this->i_s);
            }
        }

    }

    vector<pair<uint64_t, uint64_t>> down_last() {
        // SPO
        if (this->index_name.compare("SPO") == 0) {
            this->current_level += 1;
            this->i_o = this->graph->down_S_P(this->i_p, this->cur_s, this->cur_p);
            return this->graph->all_O_in_range(this->i_o);
        }

        // SOP
        else if (this->index_name.compare("SOP") == 0) {
            this->current_level += 1;
            this->i_p = this->graph->down_S_O(this->i_o, this->cur_o);
            return this->graph->all_P_in_range(this->i_p);
        }

        // POS
        else if (this->index_name.compare("POS") == 0) {
            this->current_level += 1;
            this->i_s = this->graph->down_P_O(this->i_o, this->cur_p, this->cur_o);
            return this->graph->all_S_in_range(this->i_s);
        }

        // PSO
        else if (this->index_name.compare("PSO") == 0) {
            this->current_level += 1;
            this->i_o = this->graph->down_P_S(this->i_s, this->cur_s);
            return this->graph->all_O_in_range(this->i_o);
        }

        // OSP
        else if (this->index_name.compare("OSP") == 0) {
            this->current_level += 1;
            this->i_p = this->graph->down_O_S(this->i_s, this->cur_o, this->cur_s);
            return this->graph->all_P_in_range(this->i_p);
        }

        // OPS
        else if (this->index_name.compare("OPS") == 0) {
            this->current_level += 1;
            this->i_s = this->graph->down_O_P(this->i_p, this->cur_p);
            return this->graph->all_S_in_range(this->i_s);
        }
        vector<pair<uint64_t, uint64_t>> return_vector;
        return return_vector;

    }

    void up() {
        if (this->current_level >= 0) {
            this->current_level -= 1;
        }    
    }

    void seek(uint64_t next) {
        // SPO
        if (this->index_name.compare("SPO") == 0) {
            if (this->current_level == 0) {
                this->cur_s = this->graph->next_S(this->i_s, next);
            } else if (this->current_level == 1) {
                this->cur_p = this->graph->next_P_in_S(this->i_p, this->cur_s, next);
            } else if (this->current_level == 2) {
                this->cur_o = this->graph->next_O_in_SP(this->i_o, next);
            }
        }

        // SOP
        else if (this->index_name.compare("SOP") == 0) {
            if (this->current_level == 0) {
                this->cur_s = this->graph->next_S(this->i_s, next);
            } else if (this->current_level == 1) {
                this->cur_o = this->graph->next_O_in_S(this->i_o, next);
            } else if (this->current_level == 2) {
                this->cur_p = this->graph->next_P_in_SO(this->i_p, next);
            }
        }

        // POS
        else if (this->index_name.compare("POS") == 0) {
            if (this->current_level == 0) {
                this->cur_p = this->graph->next_P(this->i_p, next);
            } else if (this->current_level == 1) {
                this->cur_o = this->graph->next_O_in_P(this->i_o, this->cur_p, next);
            } else if (this->current_level == 2) {
                this->cur_s = this->graph->next_S_in_PO(this->i_s, next);
            }
        }

        // PSO
        else if (this->index_name.compare("PSO") == 0) {
            if (this->current_level == 0) {
                this->cur_p = this->graph->next_P(this->i_p, next);
            } else if (this->current_level == 1) {
                this->cur_s = this->graph->next_S_in_P(this->i_s, next);
            } else if (this->current_level == 2) {
                this->cur_o = this->graph->next_O_in_PS(this->i_o, next);
            }
        }

        // OSP
        else if (this->index_name.compare("OSP") == 0) {
            if (this->current_level == 0) {
                this->cur_o = this->graph->next_O(this->i_o, next);
            } else if (this->current_level == 1) {
                this->cur_s = this->graph->next_S_in_O(this->i_s, this->cur_o, next);
            } else if (this->current_level == 2) {
                this->cur_p = this->graph->next_P_in_OS(this->i_p, next);
            }
        }

        // OPS
        else if (this->index_name.compare("OPS") == 0) {
            if (this->current_level == 0) {
                this->cur_o = this->graph->next_O(this->i_o, next);
            } else if (this->current_level == 1) {
                this->cur_p = this->graph->next_P_in_O(this->i_p, next);
            } else if (this->current_level == 2) {
                this->cur_s = this->graph->next_S_in_OP(this->i_s, next);
            }
        }
    }

    uint64_t current_value() {
        // SPO
        if (this->index_name.compare("SPO") == 0) {
            if (this->current_level == 0) {
                return this->cur_s;
            } else if (this->current_level == 1) {
                return this->cur_p;
            } else if (this->current_level == 2) {
                return this->cur_o;
            }
        }

        // SOP
        if (this->index_name.compare("SOP") == 0) {
            if (this->current_level == 0) {
                return this->cur_s;
            } else if (this->current_level == 1) {
                return this->cur_o;
            } else if (this->current_level == 2) {
                return this->cur_p;
            }
        }

        // POS
        if (this->index_name.compare("POS") == 0) {
            if (this->current_level == 0) {
                return this->cur_p;
            } else if (this->current_level == 1) {
                return this->cur_o;
            } else if (this->current_level == 2) {
                return this->cur_s;
            }
        }

        // PSO
        if (this->index_name.compare("PSO") == 0) {
            if (this->current_level == 0) {
                return this->cur_p;
            } else if (this->current_level == 1) {
                return this->cur_s;
            } else if (this->current_level == 2) {
                return this->cur_o;
            }
        }

        // OSP
        if (this->index_name.compare("OSP") == 0) {
            if (this->current_level == 0) {
                return this->cur_o;
            } else if (this->current_level == 1) {
                return this->cur_s;
            } else if (this->current_level == 2) {
                return this->cur_p;
            }
        }

        // OPS
        if (this->index_name.compare("OPS") == 0) {
            if (this->current_level == 0) {
                return this->cur_o;
            } else if (this->current_level == 1) {
                return this->cur_p;
            } else if (this->current_level == 2) {
                return this->cur_s;
            }
        }

        return 0;
    }

};


#endif //LEAPFROG_ITERATOR_H
