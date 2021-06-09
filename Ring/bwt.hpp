/*
 * bwt.hpp
 * Copyright (C) 2020 Author removed for double-blind evaluation
 * 
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


#ifndef BWT_T
#define BWT_T

#include "Config.hpp"

using namespace std;


class bwt
 {
    bwt_type L;

    C_type *C_bv;
    C_rank_type C_rank;
    C_select_type C_select;
    C_select0_type C_select0;

    std::vector<uint64_t> v1_aux;
    std::vector<uint64_t> v2_aux;
    public:

        std::vector<uint64_t> C;
		
        bwt() {;}

	bwt(int_vector<> & _L, vector<uint64_t> & _C)  
        {
            construct_im(L, _L);

            bit_vector C_aux = bit_vector(_C[_C.size()-1]+1+_C.size(), 0);

            for (uint64_t i=0; i < _C.size(); i++) {
                C_aux[_C[i]+i] = 1;
            }

            C_bv = new C_type(C_aux);
            util::init_support(C_rank,C_bv);
            util::init_support(C_select,C_bv);
            util::init_support(C_select0,C_bv);
        }

        ~bwt() {;}

        uint64_t size()
        {
            return sdsl::size_in_bytes(L) + sdsl::size_in_bytes(*C_bv) 
                 + sdsl::size_in_bytes(C_rank) + sdsl::size_in_bytes(C_select)
                 + sdsl::size_in_bytes(C_select0);
        }

	void save(string filename)
        {
            sdsl::store_to_file(L, filename+".L");
            sdsl::store_to_file(*C_bv, filename+".C");
        }

        void load(string filename)
        {
            sdsl::load_from_file(L, filename+".L");
            C_bv = new C_type; //bit_vector;	
            sdsl::load_from_file(*C_bv, filename+".C");
            util::init_support(C_rank,C_bv);
            util::init_support(C_select,C_bv);
            util::init_support(C_select0,C_bv);
            v1_aux = std::vector<uint64_t>(L.sigma);
            v2_aux = std::vector<uint64_t>(L.sigma); 
        }

        uint64_t LF(uint64_t i)
        {
            uint64_t s = L[i];		 	
            return get_C(s) + L.rank(i, s) - 1;
        }

        uint64_t nElems(uint64_t val)
        {
            return get_C(val+1)-get_C(val);
        }

        pair<uint64_t, uint64_t> 
        backward_step(uint64_t left_end, uint64_t right_end, uint64_t value)
        {
            uint64_t s = L.rank(left_end, value);
            uint64_t e = L.rank(right_end+1, value)-1;
            return pair<uint64_t, uint64_t>(s, e);
        }

        inline uint64_t bsearch_C(uint64_t value)
        {
            uint64_t r = C_rank(C_select0(value+1));
            return r;  
        }

        inline uint64_t get_C(uint64_t v) const
        {
            return C_select(v+1)-v;
        }

        inline uint64_t ranky(uint64_t pos, uint64_t val)
        {
            return L.rank(pos, val);
        }

        inline uint64_t rank(uint64_t pos, uint64_t val)
        {
            return L.rank(get_C(pos), val);
        }

        inline uint64_t select(uint64_t _rank, uint64_t val)
        {
            return L.select(_rank, val);
        }

        inline std::pair<uint64_t,uint64_t> select_next(uint64_t pos, uint64_t val, uint64_t n_elems)
        {
            return L.select_next(get_C(pos), val, n_elems);
        }

        inline uint64_t min_in_range(uint64_t l, uint64_t r)
        {
            return L.range_minimum_query(l, r);
        }

        inline uint64_t range_next_value(uint64_t x, uint64_t l, uint64_t r)
        {
            return L.range_next_value(x, l, r);
        }
        
        std::vector<pair<uint64_t, uint64_t>>
        //inline void
        values_in_range(uint64_t pos_min, uint64_t pos_max, uint64_t sigma/*, std::vector<uint64_t> & values, uint64_t & k*/)
        { 
            //interval_symbols(L, pos_min, pos_max+1, k, values, r_i, r_j);
            return L.range_search_2d(pos_min, pos_max, 1, sigma).second;
        } 

        // backward search for pattern of length 1
        pair<uint64_t, uint64_t> backward_search_1_interval(uint64_t P) const
        {
            return pair<uint64_t, uint64_t>(get_C(P), get_C(P+1)-1);
        }

        // backward search for pattern of length 1
        pair<uint64_t, uint64_t> backward_search_1_rank(uint64_t P, uint64_t S) const
        {
            uint64_t s = L.rank(get_C(P), S);
            uint64_t e = L.rank(get_C(P+1), S);
            return pair<uint64_t, uint64_t>(s, e);
        }

	// backward search for pattern PQ of length 2
	// returns an empty interval if search is unsuccessful
        pair<uint64_t, uint64_t> 
        backward_search_2_interval(uint64_t P, pair<uint64_t, uint64_t> &I) const
        {
            return pair<uint64_t, uint64_t>(get_C(P)+I.first, get_C(P)+I.second-1);
        }

        pair<uint64_t, uint64_t> 
        backward_search_2_rank(uint64_t P, uint64_t S, pair<uint64_t, uint64_t> &I) const
        {
            uint64_t c = get_C(P);
            uint64_t s = L.rank(c+I.first, S);
            uint64_t e = L.rank(c+I.second, S);
            return pair<uint64_t, uint64_t>(s, e);			
        }

 };
#endif
