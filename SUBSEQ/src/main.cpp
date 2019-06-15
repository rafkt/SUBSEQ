//  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.


#include "subseqPredictor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <time.h>
#include <assert.h>

#include <sys/time.h>
#include <time.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<vector<int>> testQueries;
vector<int> testAnswer;



vector<vector<int>> readTestQueries(string filename){
    ifstream infile(filename);
    vector<int> query;
    vector<vector<int>> sequenceCollection;
    for(string line; getline(infile, line ); ){
        stringstream ss(line);
        string result;
        while (getline(ss, result, ' ')) {
            query.push_back(atoi(result.c_str()));
        }
        sequenceCollection.push_back(query);
        query.clear();
    }
    return sequenceCollection;
}

int main(int argc, const char * argv[])
{

    cout << argv[1] << endl;
    vector<vector<int>> testQueries = readTestQueries(argv[2]);
    auto begin= clock();
    subseqPredictor* pr = new subseqPredictor(argv[1]);
    auto end = clock();
    auto duration = ((double)(end - begin))/CLOCKS_PER_SEC;
    cout << "Train Duration (s): " << duration << endl;


    begin = clock();
    //auto start = high_resolution_clock::now();
    for(vector<int> query : testQueries){
       // if (query.size() < 2) {cout << "short query; do we except this?" << endl; continue;}
        // int size = query.size() - 1;// our offset is 1 for now
        // vector<int> finalQuery;
        // if (5 > size){ //our query length should be 5 for now
        //     vector<int> q(query.begin(), query.begin() + size);
        //     finalQuery = q;
        // }else{
        //     vector<int> q(query.begin() + (size - 5), query.begin() + size);
        //     finalQuery = q;
        // }
        // for (int i : finalQuery) cout << i << " ";
        // cout << endl;
        pr->start(&query[0], query.size());
        //break;
    }
    //auto stop = high_resolution_clock::now();
    // cout << "Duration (ms) = " << duration_cast<milliseconds>(stop-start).count()/(double)1000 << endl;
    end = clock();
    duration = ((double)(end - begin))/CLOCKS_PER_SEC;
    cout << "Memory: " << pr->get_memory() << endl;
    cout << "Test Duration (s): " << duration << endl;
    cout << "---------------------------------------" << endl;
    
    delete pr;
	//delete evaluator;
    return 0;


}
