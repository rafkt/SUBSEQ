
//  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.

#include "backwardsSearchBWT.h"
#include <assert.h>
#include <fstream>
#include "suffixArray.h"
#include <stack>
#include <algorithm>

using namespace suffixArray;


double backwardsSearchBWT::sizeInMegabytes(){
    return size_in_mega_bytes(L) +  (8 + (sigma_seperator + 1) * 8)* 0.000001/*+ size_in_mega_bytes(*LplusOne)*/;
}


backwardsSearchBWT::backwardsSearchBWT(const string filename){
    letterNode l;
    ofstream tmpOutputFile;
    int lastItemcounter = 0;
    seqNumber =0;
    stop = false;
    ifstream file(filename);
    sigma_seperator = initialise(file, 10);
    this->L = BWT2WT();
    for (int i = 0; i < this->L.size(); i++) {
        if (this->L[i] == sigma_seperator) seqNumber++;
        try {
            alphabet_tmp.at(this->L[i])->appears++; //could be avoided though a simple rank call
        } catch (out_of_range e) {
            l = new letternode();
            l->c = this->L[i];
            l->appears = 1;
            alphabet_tmp.insert({l->c, l});
        }
    }
    this->alphabetCounters = new uint64_t[sigma_seperator + 1];
    int_vector<> tmp_v(sigma_seperator + 1, 1, 0);
    for (int i = 0; i <= sigma_seperator; i++) {
        //myMap::iterator mapIt = alphabet_tmp.begin(); mapIt != alphabet_tmp.end(); mapIt++
        
        bool missing = false;

        try {
            l = alphabet_tmp.at(i);
        } catch (out_of_range e) {
            missing = true;
        }
        //cout << l->c << endl;

        if (i == 0){
            if (!missing) this->alphabetCounters[l->c] = l->appears;
            else this->alphabetCounters[i] = 0;
        }else{
            if (!missing) this->alphabetCounters[l->c] = l->appears + this->alphabetCounters[l->c - 1];
            else this->alphabetCounters[i] = this->alphabetCounters[i - 1];
        }

    }
    clearALL();
    deleteMap();
}

void backwardsSearchBWT::deleteMap(){
    letterNode l;
    for (myMap::iterator mapIt = alphabet_tmp.begin(); mapIt != alphabet_tmp.end(); mapIt++) {
        l = mapIt->second;
        delete l;
    }
    alphabet_tmp.erase(alphabet_tmp.begin(), alphabet_tmp.end());
}

backwardsSearchBWT::~backwardsSearchBWT(){
    delete[] alphabetCounters;
    deleteMap();
}

int backwardsSearchBWT::searchQuery(int* xy, int size, int& finalStartRange, int& finalEndRange){
    if (size < 1) return -1;
    int startRange, endRange;
    getRange(xy[0], startRange, endRange);
    finalStartRange = startRange;
    finalEndRange = endRange;
    if (startRange == -1 || endRange == -1) {/*cout << "NOT FOUND" << endl;*/ return -1;}
    for (int i = 1; i < size; i++){
        if (search(xy[i], startRange, endRange, finalStartRange, finalEndRange) == -1) {/*cout << "NOT FOUND" << endl;*/ return -1;}
        startRange = finalStartRange;
        endRange = finalEndRange;
    }
    //cout << "FOUND" << endl;
    return 1;
}

int backwardsSearchBWT::search(int c, int rangeStart, int rangeEnd, int& newRangeStart, int& newRangeEnd){
    if (rangeEnd - rangeStart >= 0 && rangeEnd != -1 && rangeStart != -1){
        int rankStart = L.rank(rangeStart, c);
        int rankEnd = L.rank(rangeEnd + 1, c);
        if (rankEnd - rankStart == 0) return -1; //patern not found
        // int letterPos = alphabet.select(1, c);
        // if (letterPos != c) cout << "ERROR2 : " << letterPos << " : " << c << endl;

        int range2Add = c != 0 ? alphabetCounters[c - 1] : 0;
        newRangeStart = rankStart + range2Add;
        newRangeEnd = rankEnd + range2Add - 1;
        return 1;//valid input
    }else return 0; //no valid input
}

int backwardsSearchBWT::fowawrdTraversal(int index, int& newRangeStart){
    if (index < 0) return -1;
    // if (letterPos != L[index]) cout << "ERROR1 : " << letterPos << " : " << L[index] << endl;
    int symbol = L[index];
    int range2Add = symbol != 0 ? alphabetCounters[symbol - 1] : 0;
    int rangeStart = L.rank(index + 1, symbol) + range2Add - 1;
    newRangeStart = rangeStart;
    return L[rangeStart]; 
}

void backwardsSearchBWT::getRange(int c, int& rangeStart, int& rangeEnd){

    //get the starting range in the F array

    rangeStart = -1; rangeEnd = -1;

    if (c > 0){
        if (alphabetCounters[c] - alphabetCounters[c - 1] == 0) 
            return;
    }
    else if (c == 0){
        if (alphabetCounters[0] == 0) 
            return;
    }

    rangeStart = c != 0 ? alphabetCounters[c - 1] : 0;
    rangeEnd = alphabetCounters[c] - 1;
}



void backwardsSearchBWT::neighborExpansion(vector<int> xy, int index, int rangeStart, int rangeEnd, vector<pair<int, int>>& ranges){//size should be over or equal to 2
    int newRangeStart, newRangeEnd;
    if (index == xy.size()){
        
        ranges.push_back(make_pair(rangeStart, rangeEnd));
       
        return;
    }else{
        if (xy[index] == -2){
            
            uint64_t quantity = 0;
            std::vector<uint64_t> cs(L.sigma);      // list of characters in the interval
            std::vector<uint64_t> rank_c_i(L.sigma);    // number of occurrence of character in [0 .. i-1]
            std::vector<uint64_t> rank_c_j(L.sigma);    // number of occurrence of character in [0 .. j-1]
            if (rangeStart >= 0 || rangeEnd >= 0) interval_symbols(L, rangeStart, rangeEnd + 1, quantity, cs, rank_c_i, rank_c_j);

            
            for (int it = quantity - 1; it > -1; it--){
                if (cs[it] == sigma_seperator){ continue; }
                xy[index] = cs[it]; //mapIt->first;
                
                if (search(cs[it] /*mapIt->first*/, rangeStart, rangeEnd, newRangeStart, newRangeEnd) == -1) {/*cout << "NOT FOUND" << endl;*/ return;}
                
                neighborExpansion(xy, index + 1, newRangeStart, newRangeEnd, ranges);
            }
        }else{
            if (search(xy[index], rangeStart, rangeEnd, newRangeStart, newRangeEnd) == -1) {/*cout << "NOT FOUND" << endl;*/ return;}
            rangeStart = newRangeStart;
            rangeEnd = newRangeEnd;
            if (index == xy.size() - 1)  {
                ranges.push_back(make_pair(newRangeStart, newRangeEnd));
            }
            else neighborExpansion(xy, index + 1, newRangeStart, newRangeEnd, ranges);
        }
    }
    return;
    
}

void backwardsSearchBWT::getQuickConsequents_noLplus(int rangeStart, int rangeEnd, vector<vector<int>>& consequentList, int& predictionCount, sdsl::bit_vector* consequentBits){//size should be over or equal to 2
    for (int i = rangeStart; i <= rangeEnd; i++){
        if ((*consequentBits)[i] == 1) continue;
        (*consequentBits)[i] = 1;
        vector<int> conseq;
        int symbol = L[i];
        if (symbol != sigma_seperator){
            conseq.push_back(symbol);

            int LplusOneIndex = -1;
            fowawrdTraversal(i, LplusOneIndex);
            (*consequentBits)[LplusOneIndex] = 1;
            symbol = L[LplusOneIndex];
            if (symbol != sigma_seperator) conseq.push_back(symbol);
            consequentList.push_back(conseq);
            predictionCount++;
        }
    }
    return;
}