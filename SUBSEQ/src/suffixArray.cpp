//  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.


#include "suffixArray.h"


vector<int> suffixArray::sa;
vector<int> suffixArray::T;

int suffixArray::seqCounter;
int suffixArray::alphabet_size;
int suffixArray::length;

void suffixArray::clearALL(){
    deleteCreatedFiles();
    sa.erase(sa.begin(), sa.end());
    T.erase(T.begin(), T.end());
}

void suffixArray::clean_myfile() {
    remove( "sdb.sdsl" );
    remove( "tmp.txt" );
}

void suffixArray::deleteCreatedFiles(){
    ofstream output("sdb.sdsl", std::ios::binary | std::ios::trunc);
    atexit(clean_myfile);
}


int suffixArray::compare(const void* a, const void* b){
    if (T[*(int*)a] < T[*(int*)b]) return -1;
    else if (T[*(int*)a] > T[*(int*)b]) return 1;
    else{
        if (*(int*)a + 1 >= T.size()){
            int new_a, new_b;
            new_a = T.size() - (*(int*)a) - 1;
            new_b = *(int*)b + 1;
            return compare(&new_a, &new_b);
        }
        else if (*(int*)b + 1 >= T.size()){
            int new_a, new_b;
            new_a = *(int*)a + 1;
            new_b = T.size() - (*(int*)b) - 1;
            return compare(&new_a, &new_b);
        }
        else{
            int new_a, new_b;
            new_a = *(int*)a + 1;
            new_b = *(int*)b + 1;
            return compare(&new_a, &new_b);
        }
        
    }
    return 0;
}


int suffixArray::initialise(ifstream &file, int k_fold){ // returns the number of sequences that was red



    seqCounter = 0; alphabet_size = 0; length = 0;

    string word; file >> word;

    alphabet_size = atoi(word.c_str());
	for(; file >> word;) {
		T.push_back(atoi(word.c_str()));
		if (atoi(word.c_str()) == alphabet_size) seqCounter++;
        else length++;
    }

    reverse(T.begin(), T.end());
    for (int i = 0; i < T.size(); i++) {
        sa.push_back(i);
    }
    qsort (&sa[0], sa.size(), sizeof(int), compare);
    file.close();

    

	return alphabet_size;
}

int suffixArray::getBWT(int i){
    return sa[i] != 0 ? T[sa[i] - 1] : T[T.size() - 1];
}

wt_int<> suffixArray::BWT2WT(){
    wt_int<> wt;
    int_vector<> v(sa.size(), 1, 0);
    for (int i = 0; i < sa.size(); i++) {
        v[i] = getBWT(i);
        //cout << v[i] << " ";
    }
    //cout << endl;
    //store_to_file(v, "sdb.sdsl");
    construct_im(wt, v, 0);
    //construct(wt, "sdb.sdsl", 0);
    util::clear(v);
    
    return wt;
}