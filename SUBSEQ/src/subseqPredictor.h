//  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.



#include <iostream>
#include <vector>
#include "backwardsSearchBWT.h"
#include <map>

#include <sdsl/bit_vectors.hpp>

using namespace std;

typedef map<int, double> countTable;
typedef map<vector<int>, vector<pair<int, int>>> cashed_ranges_map;

class subseqPredictor{
public:
    backwardsSearchBWT* bSBWT;
    subseqPredictor(const string);
    ~subseqPredictor();
    int getBest();
    int start(int*, int);
	double itemConfidence(int);
	int datasetSeqNumber();
	float get_memory();
private:
	sdsl::bit_vector* consequentBits;
	countTable countTable;
	bool stop;
	int prediction;
	double score;
	void push(vector<int>, int, int, int);
	void predict(int*, int, int, int, int);
    void generateSubqueries(int*, int);
    int predictionCount;
	bool threashold;
	cashed_ranges_map cashed_ranges;
};
