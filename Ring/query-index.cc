/*
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <utility>
#include <map>
#include "triple_bwt.hpp"
#include <chrono>
#include "Term.h"
#include "Triple.h"
#include "Iterator.hpp"
#include "LeapfrogOP.hpp"
#include <boost/algorithm/string.hpp>
#include "utils.hpp"
#include<map>

using namespace boost;
using namespace std;

//#include<chrono>
//#include<ctime>

using namespace std::chrono;

bool get_file_content(string filename, vector<string> & vector_of_strings)
{
    // Open the File
    ifstream in(filename.c_str());
    // Check if object is valid
    if(!in)
    {
        cerr << "Cannot open the File : " << filename << endl;
        return false;
    }
    string str;
    // Read the next line from File untill it reaches the end.
    while (getline(in, str))
    {
        // Line contains string of length > 0 then save it in vector
        if(str.size() > 0)
            vector_of_strings.push_back(str);
    }
    //Close The File
    in.close();
    return true;
}

bool is_number(string & s)
{
    return !s.empty() && find_if(s.begin(),
        s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
}

Triple* get_triple(string & s, vector<Term*> & terms_created) {
    vector<string> terms_strings;
    split(terms_strings, s, is_any_of(" "), token_compress_on);

    Term* t1;
    Term* t2;
    Term* t3;

    if (is_number(terms_strings[0])) {
        uint64_t value;
        istringstream iss(terms_strings[0]);
        iss >> value;
        t1 = new Term(value);
    } else {
        t1 = new Term(terms_strings[0]);
    }

    if (is_number(terms_strings[1])) {
        uint64_t value;
        istringstream iss(terms_strings[1]);
        iss >> value;
        t2 = new Term(value);
    } else {
        t2 = new Term(terms_strings[1]);
    }

    if (is_number(terms_strings[2])) {
        uint64_t value;
        istringstream iss(terms_strings[2]);
        iss >> value;
        t3 = new Term(value);
    } else {
        t3 = new Term(terms_strings[2]);
    }
    terms_created.push_back(t1);
    terms_created.push_back(t2);
    terms_created.push_back(t3);
    return new Triple(t1, t2, t3);
}

vector<string> get_gao(vector<Triple*> query) {
    map<string, int> varnums;
    for (Triple * triple_pattern : query) {
        if (triple_pattern->s->isVariable) {
            if (varnums.count(triple_pattern->s->varname) == 0) {
                varnums[triple_pattern->s->varname] = 1;
            }
            else {
                varnums[triple_pattern->s->varname] += 1;
            }
        }
        if (triple_pattern->p->isVariable) {
            if (varnums.count(triple_pattern->p->varname) == 0) {
                varnums[triple_pattern->p->varname] = 1;
            }
            else {
                varnums[triple_pattern->p->varname] += 1;
            }
        }
        if (triple_pattern->o->isVariable) {
            if (varnums.count(triple_pattern->o->varname) == 0) {
                varnums[triple_pattern->o->varname] = 1;
            }
            else {
                varnums[triple_pattern->o->varname] += 1;
            }
        }
    }

    vector<pair<string, int>> varnums_pairs;
    for(auto it = (varnums).cbegin(); it != (varnums).cend(); ++it) {
        varnums_pairs.push_back(pair<string, int>(it->first, varnums[it->first]));
    }

    sort((varnums_pairs).rbegin(), (varnums_pairs).rend(), compare_by_second);

    vector<string> gao;

    for (pair<string, int> my_pair : varnums_pairs) {
        gao.push_back(my_pair.first);
    }

    return gao;

}

vector<string> get_gao_min(vector<Triple*> query, triple_bwt & graph) {
    map<string, vector<float>> triple_values;
    for (Triple * triple_pattern : query) {

        bwt_interval open_interval = graph.open_PSO();
        uint64_t cur_p = graph.min_P(open_interval);
        cur_p = graph.next_P(open_interval, triple_pattern->p->constant);

        bwt_interval i_p = graph.down_P(cur_p);

        triple_values[triple_pattern->s->varname].push_back((float)i_p.size()/query.size());
        triple_values[triple_pattern->o->varname].push_back((float)i_p.size()/query.size());
    }

    vector<pair<string, float>> varmin_pairs;
    vector<string> single_vars;
    for(auto it = (triple_values).cbegin(); it != (triple_values).cend(); ++it) {
        if (triple_values[it->first].size() == 1) {
            single_vars.push_back(it->first);
        } else {
            varmin_pairs.push_back(pair<string, float>(it->first, *min_element(triple_values[it->first].begin(), triple_values[it->first].end())));
        }
    }


    sort((varmin_pairs).begin(), (varmin_pairs).end(), compare_by_second);

    vector<string> gao;

    for (pair<string, int> my_pair : varmin_pairs) {
        gao.push_back(my_pair.first);
    }

    for (string s : single_vars) {
        gao.push_back(s);
    }

    return gao;

}

void set_scores(vector<Triple*>& query, vector<string>& gao) {
    for (Triple* triple_pattern : query) {
        triple_pattern->set_scores(gao);
    }
}

int main(int argc, char* argv[])
{
    vector<string> dummy_queries;
    bool result = get_file_content(argv[2], dummy_queries);

    triple_bwt graph;

    cout << " Loading the index..."; fflush(stdout);
    graph.load(string(argv[1]));

    cout << endl << " Index loaded " << graph.size() << " bytes" << endl;

    std::ifstream ifs;
    uint64_t nQ = 0;

    high_resolution_clock::time_point start, stop;
    double total_time = 0.0;
    duration<double> time_span;

    if(result)
    {

        int count = 1;
        for (string query_string : dummy_queries) {

            vector<Term*> terms_created;
            vector<Triple*> query;

            vector<string> tokens_query;
            split(tokens_query, query_string, is_any_of("."), token_compress_on);

            for (string token : tokens_query) {
                trim(token);
                Triple* triple_pattern = get_triple(token, terms_created);
                query.push_back(triple_pattern);
            }

            // vector<string> gao = get_gao(query);
            // vector<string> gao = get_gao_min_opt(query, graph);
            // cout << gao [0] << " - " << gao [1] << " - " << gao[2] << endl;

            start = high_resolution_clock::now();
            
            vector<string> gao = get_gao_min_gen(query, graph);
            set_scores(query, gao);

            LeapfrogOP lf(&gao, &graph, &query);

            /*
            cout << "Query Details:" << endl;
            lf.print_query();
            lf.print_gao();
            lf.serialize();
            cout << "##########" << endl;
            */

            map<string, uint64_t> bindings;
            int number_of_results = 0;

            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            lf.evaluate(0, &bindings, &number_of_results, begin);
            //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            stop = high_resolution_clock::now();
            time_span = duration_cast<microseconds>(stop - start);
            total_time = time_span.count();

            cout << nQ <<  ";" << number_of_results << ";" << (unsigned long long)(total_time*1000000000ULL) << endl;
            nQ++;

            // cout << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << std::endl;

            //cout << "RESULTS QUERY " << count << ": " << number_of_results << endl;

            // Delete pointers and empty vectors
            for (Triple* triple_pattern : query) {
                delete triple_pattern;
            }

            for (Term* term : terms_created) {
                delete term;
            }
        count += 1;
        }

    }

	return 0;
}

