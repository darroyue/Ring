g++ -O9 -std=c++11 -I ~/include -L ~/lib build-index.cpp -o build-index -lsdsl -ldivsufsort -ldivsufsort64
g++ -O9 -std=c++11 -DNODEBUG -I ~/include -L ~/lib query-index.cc Triple.cpp Term.cpp -o query-index -lsdsl -ldivsufsort -ldivsufsort64
