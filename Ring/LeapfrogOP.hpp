#ifndef LEAPFROG_OP_H
#define LEAPFROG_OP_H

#include <iostream>
#include <map>
#include <chrono>
#include "Triple.h"
#include "triple_bwt.hpp"
#include "Iterator.hpp"

class LeapfrogOP {
public:
    vector<string>* gao;
    vector<Triple*>* query;
    triple_bwt* graph;
    map<string, vector<Iterator*>> query_iterators;
    vector<Iterator*> all_iterators;
    bool is_empty;
    
    LeapfrogOP(vector<string>* gao, triple_bwt* graph, vector<Triple*>* query) {
        this->gao = gao;
        this->graph = graph;
        this->query = query;
        this->is_empty = false;
        

        for (Triple* triple_pattern : *query) {
            Iterator* triple_iterator = new Iterator(triple_pattern, graph);
            if (triple_iterator->is_empty) {
                this->is_empty = true;
            }
            for (string varname : (*this->gao)) {
                if (triple_pattern->contains_variable(varname)) {
                    this->query_iterators[varname].push_back(triple_iterator);
                }
            }
            all_iterators.push_back(triple_iterator);
        }
    }

    ~LeapfrogOP() {
        for (Iterator* triple_iterator : all_iterators) {
            delete triple_iterator;
        }
    }

    void print_query() {
        cout << "Query: " << endl;
        for (Triple* triple : *this->query) {
            triple->serialize_as_triple_pattern();
        }
    }

    void print_gao() {
        cout << "GAO: " << endl;
        for (string varname : (*this->gao)) {
            cout << varname << " ";
        }
        cout << endl;
    }

    void serialize() {
        cout << "QUERY ITERATORS: " << endl;
        for (string varname : (*this->gao)) {
            cout << "VAR:" << varname << endl;
            for (Iterator* triple_iterator : this->query_iterators[varname]) {
                cout << "Triple pattern: ";
                triple_iterator->triple->serialize_as_triple_pattern();
                cout << "Index name: " << triple_iterator->index_name << endl;
            }
        }
    }

    void evaluate(int level, map<string, uint64_t>* bindings, int * number_of_results, std::chrono::steady_clock::time_point begin) {
        if (this->is_empty) {
            return;
        }
        string varname = (*this->gao)[level];
        vector<Iterator*>* var_iterators = &this->query_iterators[varname];

        if ((*var_iterators).size() == 1 && ((*var_iterators)[0]->current_level == 1)) {
            Iterator* triple_iterator = (*var_iterators)[0];
            vector<pair<uint64_t, uint64_t>> iterator_last = triple_iterator->down_last();
            // cout << "SIZE: " << varname << " " << iterator_last.size() << endl;
            for (pair<uint64_t, uint64_t> binding_last : iterator_last) {
                // Limit
                if (*number_of_results >= 1000) {
                    break;
                }
                // TO
                std::chrono::steady_clock::time_point to_time = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds> (to_time - begin).count() >= 600) {
                    break;
                }
                if (level >= (this->gao->size() - 1)) {
                    // Print Answers
                    (*bindings)[varname] = binding_last.second;
                    
                    /*
                    for(auto it = (*bindings).cbegin(); it != (*bindings).cend(); ++it) {
                        cout << "(" << it->first << ": " << (*bindings)[it->first] << ") ";
                    }
                    cout << endl;
                    */
                    (*number_of_results) = (*number_of_results) + 1;

                } else {
                    (*bindings)[varname] = binding_last.second;
                    int new_level = level + 1;
                    evaluate(new_level, bindings, number_of_results, begin);
                }
            }
            for (Iterator* triple_iterator : *var_iterators) {
                triple_iterator->up();
            }
        }
        else {
            for (Iterator* triple_iterator : *var_iterators) {
                triple_iterator->down();
            }
            bool search = true;
            while (search) {
                // Limit
                if (*number_of_results >= 1000) {
                    for (Iterator* triple_iterator : *var_iterators) {
                        triple_iterator->up();
                    }
                    break;
                }
                // TO
                std::chrono::steady_clock::time_point to_time = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds> (to_time - begin).count() >= 600) {
                    for (Iterator* triple_iterator : *var_iterators) {
                        triple_iterator->up();
                    }
                    break;
                }
                sort((*var_iterators).rbegin(), (*var_iterators).rend(), compare_by_current_value);
                if ((*var_iterators)[0]->current_value() == (*var_iterators)[var_iterators->size() - 1]->current_value()) {
                    if (level >= (this->gao->size() - 1)) {
                        // Print Answers
                        (*bindings)[varname] = (*var_iterators)[0]->current_value();
                        
                        /*
                        for(auto it = (*bindings).cbegin(); it != (*bindings).cend(); ++it) {
                            cout << "(" << it->first << ": " << (*bindings)[it->first] << ") ";
                        }
                        cout << endl;
                        */
                    
                        (*number_of_results) = (*number_of_results) + 1;
                        int next_value = (*var_iterators)[0]->current_value() + 1;
                        (*var_iterators)[0]->seek(next_value);
                    } else {
                        (*bindings)[varname] = (*var_iterators)[0]->current_value();
                        int new_level = level + 1;
                        evaluate(new_level, bindings, number_of_results, begin);
                        int next_value = (*var_iterators)[0]->current_value() + 1;
                        (*var_iterators)[0]->seek(next_value);
                    }
                }

                int seek_value = (*var_iterators)[0]->current_value();
                if (seek_value != 0) {
                    for (int i = 1; i < var_iterators->size(); i++) {
                        (*var_iterators)[i]->seek(seek_value);
                    }
                }

                for (Iterator* triple_iterator : (*var_iterators)) {
                    if (triple_iterator->current_value() == 0) {
                        for (Iterator* triple_iterator : *var_iterators) {
                            triple_iterator->up();
                        }
                        search = false;  
                    }          
                }

            }
        }
        
    }

    static bool compare_by_current_value(Iterator* a, Iterator* b) {
        return a->current_value() < b->current_value();
    }

};


#endif //LEAPFROG_ITERATOR_H
