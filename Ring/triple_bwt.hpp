/*
 * triple_bwt.hpp
 * Copyright (C) 2020 Author name removed for double blind evaluation
 * 
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

#ifndef TRIPLE_BWT
#define TRIPLE_BWT

#include <cstdint>
#include "bwt.hpp"
#include "bwt_interval.hpp"

#include <stdio.h>
#include <stdlib.h>


class triple_bwt 
 {
    bwt BWT_S;
    bwt BWT_P;
    bwt BWT_O;

    uint64_t max_S;
    uint64_t max_P;
    uint64_t max_O;
    uint64_t nTriples;  // number of triples
    uint64_t sigma_S;
    uint64_t sigma_P;
    uint64_t sigma_O;    

    public:
        triple_bwt() {;}

        // Assumes the triples have been stored in a vector<spo_triple>
        triple_bwt(vector<spo_triple> & D)
        {
            uint64_t i, pos_c;
            vector<spo_triple>::iterator it, triple_begin = D.begin(), triple_end = D.end();
            uint64_t n = nTriples = triple_end - triple_begin;
            int_vector<32> bwt_aux(3*n);

            //cout << "  > Determining alphabets of S; P; O..."; fflush(stdout); 
            {
                set<uint64_t> alphabet_S, alphabet_P, alphabet_O;
                for (it = triple_begin, i=0; i<n; i++, it++) {
                    alphabet_S.insert(std::get<0>(*it));
                    alphabet_P.insert(std::get<1>(*it));
                    alphabet_O.insert(std::get<2>(*it));
                } 

                //cout << "Done" << endl; fflush(stdout);
                sigma_S = alphabet_S.size();
                sigma_P = alphabet_P.size();
                sigma_O = alphabet_O.size();
                max_S = *alphabet_S.rbegin();
                max_P = *alphabet_P.rbegin();
                max_O = *alphabet_O.rbegin();
                alphabet_S.clear(); 
                alphabet_P.clear(); 
                alphabet_O.clear(); 
            }
            //cout << "sigma S = " << sigma_S << endl;
            //cout << "sigma P = " << sigma_P << endl;
            //cout << "sigma O = " << sigma_O << endl;
            //cout << "  > Determining number of elements per symbol..."; fflush(stdout);
            uint64_t alphabet_SO = (max_S < max_O)? max_O: max_S;

            std::map<uint64_t, uint64_t> M_O, M_S, M_P;

            for (i = 1; i <= alphabet_SO; i++) {
                M_O[i] = 0;
                M_S[i] = 0;
            }

            for (i = 1; i <= max_P; i++)
                M_P[i] = 0;

            for (it = triple_begin, i=0; i<n; i++, it++) {
                M_O[std::get<2>(*it)] = M_O[std::get<2>(*it)] + 1;
                M_S[std::get<0>(*it)] = M_S[std::get<0>(*it)] + 1; 
                M_P[std::get<1>(*it)] = M_P[std::get<1>(*it)] + 1;             
            }
            //cout << "Done" << endl; fflush(stdout);            
            //cout << "  > Sorting out triples..."; fflush(stdout);
            // Sorts the triples lexycographically 
            sort(triple_begin, triple_end);
            //cout << "Done" << endl; fflush(stdout);
            {
	        int_vector<> t(3*n+2);
                //cout << "  > Generating int vector of the triples..."; fflush(stdout);
                for (i=0, it=triple_begin; it != triple_end; it++, i++) {
                    t[3*i]   = std::get<0>(*it);
                    t[3*i+1] = std::get<1>(*it) + max_S;  
                    t[3*i+2] = std::get<2>(*it) + max_S + max_P; 
                }
                t[3*n] = max_S + max_P + max_O + 1;
                t[3*n+1] = 0;
                D.clear();
                D.shrink_to_fit();
                //cout << "Size of D " << D.size() << " elements" << endl;
                util::bit_compress(t);
	        //cout << "Done" << endl; fflush(stdout);
                //cout << "t size " << size_in_bytes(t) << endl;
                //int_vector<> bwt_aux(3*n);    
            
                {
                    int_vector<> sa;
		    //cout << "  > Building the suffix array" << endl;
 
		    qsufsort::construct_sa(sa, t);
                                 
		    //cout << "  > Suffix array built " << size_in_bytes(sa) << " bytes" <<  endl;
		    //cout << "  > Building the global BWT" << endl;

                    uint64_t j;
		    for (j=i=0; i < sa.size(); i++) {
		        if (sa[i] >= 3*n) continue;
			if (sa[i] == 0)
			    bwt_aux[j] = t[t.size()-3];
			else bwt_aux[j] = t[sa[i]-1];
			j++;
		    }
                }
            }
            //cout << "  > Building BWT_O" << endl; fflush(stdout);
	    // First O
            {
                int_vector<> O(n+1);
                uint64_t j = 1, c, c_aux;
                vector<uint64_t> C_O;
                O[0] = 0;
                for (i=1; i<n; i++)
                    O[j++] = bwt_aux[i] - (max_S + max_P); 
				
                // This is for making the bwt of triples circular
                O[j] = bwt_aux[0] - (max_S + max_P);				
                util::bit_compress(O);
				
                uint64_t cur_pos = 1;

                C_O.push_back(0); // Dummy value
                C_O.push_back(cur_pos);
                for (c = 2; c <= alphabet_SO; c++) {
                    cur_pos += M_S[c-1];
                    C_O.push_back(cur_pos); 
                }
                C_O.push_back(n+1);                
                C_O.shrink_to_fit();
		
                M_S.clear();
	
                // builds the WT for BWT(O) 
                BWT_O = bwt(O, C_O);
            }

            //cout << "  > Building BWT_S" << endl; fflush(stdout);
            // Then S
            {
                int_vector<> S(n+1);
                uint64_t j = 1, c;
                vector<uint64_t> C_S;

                S[0] = 0;
                while (i<2*n) {
                    S[j++] = bwt_aux[i++];  
                }
                util::bit_compress(S);

                uint64_t cur_pos = 1;
                C_S.push_back(0);  // Dummy value
                C_S.push_back(cur_pos);
                for (c = 2; c <= max_P; c++) {
                    cur_pos += M_P[c-1];
                    C_S.push_back(cur_pos); 
                }
                C_S.push_back(n+1);
                C_S.shrink_to_fit();

                M_P.clear();

                BWT_S = bwt(S, C_S);
            }

            //cout << "  > Building BWT_P" << endl; fflush(stdout);	
            // Then P
            {
                int_vector<> P(n+1);
                uint64_t j = 1, c;
                vector<uint64_t> C_P;

                P[0] = 0;
                while (i<3*n) {
                    P[j++] = bwt_aux[i++] - max_S;  
                }
                util::bit_compress(P);

                uint64_t cur_pos = 1;
                C_P.push_back(0);  // Dummy value
                C_P.push_back(cur_pos);
                for (c = 2; c <= alphabet_SO; c++) {
                    cur_pos += M_O[c-1];
                    C_P.push_back(cur_pos); 
                }
                C_P.push_back(n+1);
                C_P.shrink_to_fit();
                
		M_O.clear();

                BWT_P = bwt(P, C_P);				
            }
            cout << "-- Index constructed successfully" << endl; fflush(stdout);
        };

        uint64_t size()
        {
            //cout << "BWT_O: " << (float)BWT_O.size()*8/nTriples << endl; 
            //cout << "BWT_S: " << (float)BWT_S.size()*8/nTriples << endl; 
            //cout << "BWT_P: " << (float)BWT_P.size()*8/nTriples << endl; 
            return BWT_O.size() + BWT_S.size() + BWT_P.size();
        }

	void save(string filename)
	{
	    BWT_O.save(filename+".bwto");
	    BWT_S.save(filename+".bwts");
	    BWT_P.save(filename+".bwtp");

	    std::ofstream ofs(filename+".nTriples");
	    ofs << nTriples << endl;
	    ofs << max_S << endl;
	    ofs << max_P << endl;
	    ofs << max_O << endl;
            ofs << sigma_S << endl;
            ofs << sigma_P << endl;
            ofs << sigma_O << endl;
	};

        void load(string filename)
        {
            //cout << "Loading BWT_O" << endl; fflush(stdout);
            BWT_O.load(filename+".bwto"); 
            //cout << "Loading BWT_S" << endl; fflush(stdout);
            BWT_S.load(filename+".bwts");
            //cout << "Loading BWT_P" << endl; fflush(stdout);
            BWT_P.load(filename+".bwtp");
            //cout << "Loading done" << endl; fflush(stdout);
            std::ifstream ifs(filename+".nTriples");
            ifs >> nTriples;
            ifs >> max_S;
            ifs >> max_P;
            ifs >> max_O;
            ifs >> sigma_S;
            ifs >> sigma_P;
            ifs >> sigma_O;
            //cout << "sigma_S="<<sigma_S<<endl;
            //cout << "sigma_P="<<sigma_P<<endl;
            //cout << "sigma_O="<<sigma_O<<endl;
            //cout << "max_S="<<max_S<<endl;
            //cout << "max_P="<<max_P<<endl;
            //cout << "max_O="<<max_O<<endl;
        };

        // The following init funtions work with suffix array positions
        // (i.e., positions in the global interval [1, 3*nTriples] )
        pair<uint64_t, uint64_t> init_no_constants() const
        {
            return pair<uint64_t, uint64_t>(1,3*nTriples);
        }

        pair<uint64_t, uint64_t> init_S(uint64_t S) const
        {
            return BWT_O.backward_search_1_interval(S);			
        }

        pair<uint64_t, uint64_t> init_P(uint64_t P) const
        {
            pair<uint64_t, uint64_t> I = BWT_S.backward_search_1_interval(P);			
            return pair<uint64_t, uint64_t>(I.first+nTriples, I.second+nTriples); 
        }

        pair<uint64_t, uint64_t> init_O(uint64_t O) const
        {
            pair<uint64_t, uint64_t> I = BWT_P.backward_search_1_interval(O);
            return pair<uint64_t, uint64_t>(I.first+2*nTriples, I.second+2*nTriples);			
        }

        pair<uint64_t, uint64_t> init_SP(uint64_t S, uint64_t P) const
        {
            pair<uint64_t, uint64_t> I;
            I = BWT_S.backward_search_1_rank(P, S);
            return BWT_O.backward_search_2_interval(S, I);
        }

        pair<uint64_t, uint64_t> init_SO(uint64_t S, uint64_t O) const
        {
            pair<uint64_t, uint64_t> I;
            I = BWT_O.backward_search_1_rank(S, O);
            I = BWT_P.backward_search_2_interval(O, I);
            return pair<uint64_t, uint64_t>(I.first+2*nTriples, I.second+2*nTriples);			
        }

        pair<uint64_t, uint64_t> init_PO(uint64_t P, uint64_t O) const
        {
            pair<uint64_t, uint64_t> I;
            I = BWT_P.backward_search_1_rank(O, P);
            I = BWT_S.backward_search_2_interval(P, I);
            return pair<uint64_t, uint64_t>(I.first+nTriples, I.second+nTriples);			
        }

        pair<uint64_t, uint64_t> init_SPO(uint64_t S, uint64_t P, uint64_t O) const
        {
            pair<uint64_t, uint64_t> I;
            I = BWT_P.backward_search_1_rank(O, P);
            I = BWT_S.backward_search_2_rank(P, S, I);
            return BWT_O.backward_search_2_interval(S, I);
        }

        /**********************************/
        // Functions for PSO
        //

        bwt_interval open_PSO()
        {
            return bwt_interval(2*nTriples+1, 3*nTriples);
        }
        
        /**********************************/
        // P->S  (simulates going down in the trie)
        // Returns an interval within BWT_O
        bwt_interval down_P_S(bwt_interval & p_int, uint64_t s)
        {
            pair<uint64_t, uint64_t> I = BWT_S.backward_step(p_int.left()-nTriples, p_int.right()-nTriples, s);
            uint64_t c = BWT_O.get_C(s);
            return bwt_interval(I.first+c, I.second+c);
        }

        uint64_t min_O_in_S(bwt_interval & I)
        {
            return I.begin(BWT_O);
        }

        uint64_t next_O_in_S(bwt_interval & I, uint64_t O)
        {
            if (O > max_O) return 0;
 
            uint64_t nextv = I.next_value(O, BWT_O);
            if (nextv==0) 
                return 0;
            else
                return nextv;
        }

        bool there_are_O_in_S(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        uint64_t min_O_in_PS(bwt_interval & I)
        {
            return I.begin(BWT_O);
        }

        uint64_t next_O_in_PS(bwt_interval & I, uint64_t O)
        {
            if (O > max_O) return 0;

            uint64_t nextv = I.next_value(O, BWT_O);      
            if (nextv == 0) 
                return 0;            
            else
                return nextv; 
        }

        bool there_are_O_in_PS(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        std::vector<std::pair<uint64_t, uint64_t>>
	all_O_in_range(bwt_interval & I)
        {
            return BWT_O.values_in_range(I.left(), I.right(), sigma_S+sigma_O); 
        }

        /**********************************/
        // Functions for OPS
        //

        bwt_interval open_OPS()
        {
            return bwt_interval(1, nTriples);
        }

        /**********************************/
        // O->P  (simulates going down in the trie)
        // Returns an interval within BWT_S
        bwt_interval down_O_P(bwt_interval & o_int, uint64_t p)
        {
            pair<uint64_t, uint64_t> I = BWT_P.backward_step(o_int.left()-2*nTriples, o_int.right()-2*nTriples, p);
            uint64_t c = BWT_S.get_C(p);
            return bwt_interval(I.first+c+nTriples, I.second+c+nTriples);
        }

        uint64_t min_S_in_OP(bwt_interval & I)
        {
            bwt_interval I_aux(I.left()-nTriples, I.right()-nTriples);            
            return I_aux.begin(BWT_S);
        }

        uint64_t next_S_in_OP(bwt_interval & I, uint64_t s_value)
        {
            if (s_value > max_S) return 0;

            bwt_interval I_aux(I.left()-nTriples, I.right()-nTriples);
            uint64_t nextv = I_aux.next_value(s_value, BWT_S); 
            if (nextv==0)
                return 0;              
            else
                return nextv;
        }

        bool there_are_S_in_OP(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        uint64_t min_S_in_P(bwt_interval & I)
        {
            bwt_interval I_aux(I.left()-nTriples, I.right()-nTriples);            
            return I_aux.begin(BWT_S);
        }

        uint64_t next_S_in_P(bwt_interval & I, uint64_t s_value)
        {
            if (s_value > max_S) return 0;

            bwt_interval I_aux(I.left()-nTriples, I.right()-nTriples);
            uint64_t nextv = I_aux.next_value(s_value, BWT_S);
            if (nextv==0)
                return 0;              
            else
                return nextv;
        }

        bool there_are_S_in_P(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        std::vector<std::pair<uint64_t, uint64_t>>
	all_S_in_range(bwt_interval & I)
        {
	    return BWT_S.values_in_range(I.left()-nTriples, I.right()-nTriples, sigma_S+sigma_O); 
        }


        /**********************************/
        // Function for SOP
        //

        bwt_interval open_SOP()
        {
            return bwt_interval(nTriples+1, 2*nTriples);
        }

        /**********************************/
        // S->O  (simulates going down in the trie)
        // Returns an interval within BWT_P
        bwt_interval down_S_O(bwt_interval & s_int, uint64_t o)
        {
            pair<uint64_t, uint64_t> I = BWT_O.backward_step(s_int.left(), s_int.right(), o);
            uint64_t c = BWT_P.get_C(o);
            return bwt_interval(I.first+c+2*nTriples, I.second+c+2*nTriples);
        }
 
        uint64_t min_P_in_SO(bwt_interval & I)
        {
            bwt_interval I_aux(I.left()-2*nTriples, I.right()-2*nTriples);
            return I_aux.begin(BWT_P);
        }

        uint64_t next_P_in_SO(bwt_interval & I, uint64_t p_value)
        {
            if (p_value > max_P) return 0;

            bwt_interval I_aux(I.left()-2*nTriples, I.right()-2*nTriples);
            uint64_t nextv = I_aux.next_value(p_value, BWT_P);             
            if (nextv==0)
                return 0;
            else
                return nextv;
        }

        bool there_are_P_in_SO(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }
 
        uint64_t min_P_in_O(bwt_interval & I)
        {
            bwt_interval I_aux(I.left()-2*nTriples, I.right()-2*nTriples);
            return I_aux.begin(BWT_P);
        }

        uint64_t next_P_in_O(bwt_interval & I, uint64_t p_value)
        {
            if (p_value > max_P) return 0;

            bwt_interval I_aux(I.left()-2*nTriples, I.right()-2*nTriples);
            uint64_t nextv = I_aux.next_value(p_value, BWT_P);             
            if (nextv==0) 
                return 0;
            else
                return nextv;
        }

        bool there_are_P_in_O(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        std::vector<std::pair<uint64_t, uint64_t>>
	all_P_in_range(bwt_interval & I)
        {
	    return BWT_P.values_in_range(I.left()-2*nTriples, I.right()-2*nTriples, sigma_P); 
        }


        /**********************************/
        // Functions for SPO
        //
        bwt_interval open_SPO()
        {
            return bwt_interval(nTriples+1, 2*nTriples);
        }

        uint64_t min_S(bwt_interval & I)
        {
            bwt_interval I_aux = bwt_interval(I.left()-nTriples, I.right()-nTriples);
            return I_aux.begin(BWT_S);        
        }

        uint64_t next_S(bwt_interval & I, uint64_t s_value)
        {
            if (s_value > max_S) return 0;

            bwt_interval I_aux = bwt_interval(I.left()-nTriples, I.right()-nTriples);            
            return I_aux.next_value(s_value, BWT_S);
        }

        bwt_interval down_S(uint64_t s_value)
        {
            pair<uint64_t, uint64_t> i = init_S(s_value);
            return bwt_interval(i.first, i.second);
        }        
        
        // S->P  (simulates going down in the trie, for the order SPO)
        // Returns an interval within BWT_P
        bwt_interval down_S_P(bwt_interval & s_int, uint64_t s_value, uint64_t p_value)
        { 
            std::pair<uint64_t, uint64_t> q = s_int.get_stored_values();            
            uint64_t b = q.first;            
            if (q.first == (uint64_t)-1) {
                q = BWT_S.select_next(p_value, s_value, BWT_O.nElems(s_value));
                b = BWT_S.bsearch_C(q.first) - 1;
            }
            uint64_t nE = BWT_S.rank(b+1, s_value) - BWT_S.rank(b, s_value);
            uint64_t start = q.second;

            return bwt_interval(s_int.left()+start, s_int.left()+start+nE-1);
        }

        uint64_t min_P_in_S(bwt_interval & I, uint64_t s_value)
        {
            std::pair<uint64_t, uint64_t> q;
            q = BWT_S.select_next(1, s_value, BWT_O.nElems(s_value));
            uint64_t b = BWT_S.bsearch_C(q.first) - 1;
            I.set_stored_values(b, q.second); 
            return b;  
        }

        uint64_t next_P_in_S(bwt_interval & I, uint64_t s_value, uint64_t p_value)
        {  
            if (p_value > max_P) return 0;

            std::pair<uint64_t, uint64_t> q;
            q = BWT_S.select_next(p_value, s_value, BWT_O.nElems(s_value));
            if (q.first == 0 && q.second == 0) {
		return 0;
            }

            uint64_t b = BWT_S.bsearch_C(q.first) - 1;
            I.set_stored_values(b, q.second);
            return b;  
        }


        uint64_t min_O_in_SP(bwt_interval & I)
        {
            return I.begin(BWT_O);
        }

        uint64_t next_O_in_SP(bwt_interval & I, uint64_t O)
        {
            if (O > max_O) return 0;

            uint64_t next_v = I.next_value(O, BWT_O);
            if (next_v == 0) 
                return 0;            
            else 
                return next_v;
        }

        bool there_are_O_in_SP(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        /**********************************/
        // Functions for POS
        //

        bwt_interval open_POS()
        {
            return bwt_interval(2*nTriples+1, 3*nTriples);
        }

        uint64_t min_P(bwt_interval & I)
        {
            bwt_interval I_aux(I.left()-2*nTriples, I.right()-2*nTriples);
            return I_aux.begin(BWT_P);        
        }

        uint64_t next_P(bwt_interval & I, uint64_t p_value)
        {
            if (p_value > max_P) return 0;
            
            bwt_interval I_aux(I.left()-2*nTriples, I.right()-2*nTriples);
            uint64_t nextv = I_aux.next_value(p_value, BWT_P);             
            if (nextv == 0)
                return 0;
            else
                return nextv;
        }

        bwt_interval down_P(uint64_t p_value)
        {
            pair<uint64_t, uint64_t> i = init_P(p_value);
            return bwt_interval(i.first, i.second);
        }        

        // P->O  (simulates going down in the trie, for the order POS)
        // Returns an interval within BWT_P
        bwt_interval down_P_O(bwt_interval & p_int, uint64_t p_value, uint64_t o_value)
        {
            std::pair<uint64_t, uint64_t> q = p_int.get_stored_values();
            uint64_t b = q.first;
            if (q.first == (uint64_t)-1) {
                q = BWT_P.select_next(o_value, p_value, BWT_S.nElems(p_value));
                b = BWT_P.bsearch_C(q.first) - 1;
            }            
            uint64_t nE = BWT_P.rank(b+1, p_value) - BWT_P.rank(b, p_value);
            uint64_t start = q.second;
 
            return bwt_interval(p_int.left()+start, p_int.left()+start+nE-1); 
        }

        uint64_t min_O_in_P(bwt_interval & p_int, uint64_t p_value)
        {
            std::pair<uint64_t, uint64_t> q;
            q = BWT_P.select_next(1, p_value, BWT_S.nElems(p_value));
            uint64_t b = BWT_P.bsearch_C(q.first) - 1;             
            p_int.set_stored_values(b, q.second);            
            return b;
        }

        uint64_t next_O_in_P(bwt_interval & I, uint64_t p_value, uint64_t o_value)
        {
            if (o_value > max_O) return 0;

            std::pair<uint64_t, uint64_t> q;
            q = BWT_P.select_next(o_value, p_value, BWT_S.nElems(p_value));
            if (q.first == 0 && q.second == 0)
                return 0;
            
            uint64_t b = BWT_P.bsearch_C(q.first) - 1; 
            I.set_stored_values(b, q.second);            
            return b;
        }

        uint64_t min_S_in_PO(bwt_interval & I)
        {
            bwt_interval I_aux = bwt_interval(I.left()-nTriples, I.right()-nTriples);
            return I_aux.begin(BWT_S);
        }

        uint64_t next_S_in_PO(bwt_interval & I, uint64_t s_value)
        {
            if (s_value > max_S) return 0;  

            bwt_interval I_aux = bwt_interval(I.left()-nTriples, I.right()-nTriples);
            return I_aux.next_value(s_value, BWT_S);
        }

        bool there_are_S_in_PO(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }

        /**********************************/
        // Functions for OSP
        //

        bwt_interval open_OSP()
        {
            return bwt_interval(1, nTriples);
        }

        uint64_t min_O(bwt_interval & I)
        {
            return I.begin(BWT_O);        
        }

        uint64_t next_O(bwt_interval & I, uint64_t o_value)
        {
            if (o_value > max_O) return 0;

            uint64_t nextv = I.next_value(o_value, BWT_O);
            if (nextv == 0) return 0;            
            else return nextv;
        }

        bwt_interval down_O(uint64_t o_value)
        {
            pair<uint64_t, uint64_t> i = init_O(o_value);
            return bwt_interval(i.first, i.second);
        }        

        // P->O  (simulates going down in the trie, for the order OSP)
        // Returns an interval within BWT_P
        bwt_interval down_O_S(bwt_interval & o_int, uint64_t o_value, uint64_t s_value)
        {
            std::pair<uint64_t, uint64_t> q = o_int.get_stored_values();            
            uint64_t b = q.first;
            if (q.first == (uint64_t)-1) {
                q = BWT_O.select_next(s_value, o_value, BWT_P.nElems(o_value));
                b = BWT_O.bsearch_C(q.first) - 1;
            }
            uint64_t nE = BWT_O.rank(b+1, o_value) - BWT_O.rank(b, o_value);
            uint64_t start = q.second;

            return bwt_interval(o_int.left()+start, o_int.left()+start+nE-1);
        }

        uint64_t min_S_in_O(bwt_interval & o_int, uint64_t o_value)
        {
            std::pair<uint64_t, uint64_t> q;
            q = BWT_O.select_next(1, o_value, BWT_P.nElems(o_value)); 
            uint64_t b = BWT_O.bsearch_C(q.first) - 1;
            o_int.set_stored_values(b, q.second); 
            return b;
        }

        uint64_t next_S_in_O(bwt_interval & I, uint64_t o_value, uint64_t s_value)
        {            
            if (s_value > max_S) return 0;
 
            std::pair<uint64_t, uint64_t> q;
            q = BWT_O.select_next(s_value, o_value, BWT_P.nElems(o_value));
            if (q.first == 0 && q.second == 0) 
                return 0;

            uint64_t b = BWT_O.bsearch_C(q.first) - 1;
            I.set_stored_values(b, q.second);
            return b;
        }

        uint64_t min_P_in_OS(bwt_interval & I)
        {
            bwt_interval I_aux = bwt_interval(I.left()-2*nTriples, I.right()-2*nTriples);
            return I_aux.begin(BWT_P);
        }

        uint64_t next_P_in_OS(bwt_interval & I, uint64_t p_value)
        {
            if (p_value > max_P) return 0;

            bwt_interval I_aux = bwt_interval(I.left()-2*nTriples, I.right()-2*nTriples);
            uint64_t nextv = I_aux.next_value(p_value, BWT_P);             
            if (nextv == 0)
                return 0;
            else            
                return nextv;
        }

        bool there_are_P_in_OS(bwt_interval & I)
        {
            return I.get_cur_value() != I.end(); 
        }
 
 };
#endif
