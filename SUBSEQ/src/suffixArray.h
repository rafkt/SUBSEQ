//  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.


#include <sdsl/wavelet_trees.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <iterator>


using namespace std;
using namespace sdsl;

namespace suffixArray {
    extern vector<int> sa;
    extern vector<int> T;
    extern int seqCounter;
    extern int alphabet_size;
    extern int length;
    
    int compare(const void*, const void*);
    int initialise(ifstream&, int);
    int getBWT(int);
    void clean_myfile();
    void deleteCreatedFiles();
    wt_int<> BWT2WT();
    void clearALL();

};
