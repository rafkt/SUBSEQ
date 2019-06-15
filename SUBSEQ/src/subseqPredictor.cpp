//  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.


#include "subseqPredictor.h"
//#include "similar_queries.h"
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <set>

#define MAXPREDICTIONCOUNT 10

float subseqPredictor::get_memory(){
	return bSBWT->sizeInMegabytes();
}

subseqPredictor::subseqPredictor(const string filename){
    bSBWT = new backwardsSearchBWT(filename);
}

subseqPredictor::~subseqPredictor(){
    delete bSBWT;
}

int compare(const void* a, const void* b){
    return ( *(int*)a - *(int*)b );
}

double subseqPredictor::itemConfidence(int item){
	//int frequency = bSBWT->L.rank(bSBWT->L.size() - 1, item);
	int frequency;
	if (item == 0){
		frequency = bSBWT->alphabetCounters[0];
	}else{
		frequency = bSBWT->alphabetCounters[item] - bSBWT->alphabetCounters[item - 1];
	}
	//cout << "Freq " << frequency << " Sequences " << bSBWT->seqNumber << endl;
	return ((double) frequency / bSBWT->seqNumber);
}

int subseqPredictor::datasetSeqNumber(){
	return bSBWT->seqNumber;
}

void subseqPredictor::predict(int* query, int size, int maxPredictionCount, int initialLength, int errors){
	
	vector<pair<int, int>> retrieved_ranges;

	int rangeStart = - 1, rangeEnd = -1;
	set<int> firstItemPossibleReplacements, secondItemPossibleReplacements;
	vector<pair<int, int>> bs_ranges;
	vector<pair<int, int>> exclude_bs_ranges;
	if (query[0] == -2){
		
		vector<int> exclude_query_vector(size);
		copy(query, query + size, exclude_query_vector.begin());


		if (errors > 1 && query[1] == -2){



					int finalStartRange, finalEndRange;
			    	if (bSBWT->searchQuery(&(query[2]), size - 2, finalStartRange, finalEndRange) != -1){
			    		bs_ranges.push_back(make_pair(finalStartRange, finalEndRange));
			    	}
			    		
		
		}else{

			exclude_query_vector[0] = bSBWT->sigma_seperator;

			//repeated code here - should improve it
			vector<int> query_vector(size - 1);
			copy(&(query[1]), query + size, query_vector.begin());
		    if (errors > 1){

				if (cashed_ranges[query_vector].size() == 0){
					bSBWT->getRange(query[1], rangeStart, rangeEnd); // -2 a b  -2 c
		    		bSBWT->neighborExpansion(query_vector, 1, rangeStart, rangeEnd, bs_ranges); // a b -2 c
		    		cashed_ranges[query_vector] = bs_ranges;
				}else{
					bs_ranges = cashed_ranges[query_vector];
				}
		    	
		    }else{

		    	int finalStartRange, finalEndRange;
		    	if (bSBWT->searchQuery(&(query[1]), size - 1, finalStartRange, finalEndRange) != -1){
		    		bs_ranges.push_back(make_pair(finalStartRange, finalEndRange));
		    	}
		    }
			//end of repeated code
		}

		//excluding ranges
		for (pair<int, int> range : exclude_bs_ranges){
			for (int r = range.first; r <= range.second; r++) { (*consequentBits)[r] = 1;}
		}
		//end of excluding ranges


		vector<vector<int>> consequentList;
	    //cout << " : Ranges : " << bs_ranges.size() << endl;
	    for (pair<int, int> it : bs_ranges){
	    	vector<int> tmp;
	    	//bSBWT->getConsequents(tmp, 0, it.first, it.second, 2, -1, consequentList, predictionCount, consequentBits);
	    	bSBWT->getQuickConsequents_noLplus(it.first, it.second, consequentList, predictionCount, consequentBits);
	    }
	    //put all ranges into CT
	    for (vector<int> consequent : consequentList) push(consequent, errors, initialLength, size);
	    
	    if (predictionCount > MAXPREDICTIONCOUNT) {
	    	stop = true;
	    	return;
		}
	}else{
		vector<int> query_vector(size);
		copy(query, query + size, query_vector.begin());
	    if (errors > 0){
	    	bSBWT->getRange(query[0], rangeStart, rangeEnd);
	    	if (rangeStart != -1 && rangeEnd != -1){
	    		if (cashed_ranges[query_vector].size() == 0){
	    			bSBWT->neighborExpansion(query_vector, 1, rangeStart, rangeEnd, bs_ranges);
	    			cashed_ranges[query_vector] = bs_ranges;
	    		}else{
	    			bs_ranges = cashed_ranges[query_vector];
	    		}
	    	}
	    }else{

	 
	    	int finalStartRange, finalEndRange;
	    	if (bSBWT->searchQuery(query, size, finalStartRange, finalEndRange) != -1){
	    		bs_ranges.push_back(make_pair(finalStartRange, finalEndRange));
	    	}
	    }
	    //cout << ": Ranges : " << bs_ranges.size() << endl;
	    vector<vector<int>> consequentList;
	    for (pair<int, int> it : bs_ranges){
	    	vector<int> tmp;
	    	//bSBWT->getConsequents(tmp, 0, it.first, it.second, 2, -1, consequentList, predictionCount, consequentBits);
	    	bSBWT->getQuickConsequents_noLplus(it.first, it.second, consequentList, predictionCount, consequentBits);
	    }
	    //put all ranges into CT
	    for (vector<int> consequent : consequentList) push(consequent, errors, initialLength, size);
	    
	    if (predictionCount > MAXPREDICTIONCOUNT) {
	    	stop = true;
	    	return;
		}
	}

	
    return;
}


void subseqPredictor::push(vector<int> consequent, int errors, int initialLength, int subLength){
	
	pair<std::map<int, double>::iterator, bool> ret;
	double current_score = 0.0;
	for (int i = 0; i < consequent.size(); i++){
		double weightDistance = 1.0 / (i + 1);
		double newWeight = ((subLength / (double)initialLength)) + ((2 - errors) / (double)2) + (1.0) + (weightDistance);
		ret = countTable.insert(std::pair<int, double>(consequent[i], newWeight));
	  	if (ret.second == false) {
	  		current_score = ret.first->second * newWeight;
		    ret.first->second =  current_score;
		    if (current_score > score){
		    	score = current_score;
		    	prediction = consequent[i];
		    }
		}else{
			if (newWeight > score){
		    	score = newWeight;
		    	prediction = consequent[i];
		    }
		}
	}
}

int subseqPredictor::getBest(){
	return prediction;
}

int subseqPredictor::start(int* query, int size){ // this function will manage deletion of the first items.
	consequentBits = new bit_vector(bSBWT->L.size(), 0); // I have to delete this at the end of this function or before returning
	cashed_ranges.clear();
	stop = false;
	prediction = -1; score = -1.0;
	predictionCount = 0;
	countTable.clear();
	if (size == 2) {
		predict(query, size, MAXPREDICTIONCOUNT, size, 0);
		if (!stop) generateSubqueries(query, size);
	} else if (size < 2) return getBest();
	for (int k = 0; k < size - 1; k++) {
		predictionCount = 0;
		predict(&query[k], size - k, MAXPREDICTIONCOUNT, size, 0);
		if (stop) {delete consequentBits; return getBest();}
		generateSubqueries(&query[k], size - k); 
		if (stop) {delete consequentBits; return getBest();}
	}
	delete consequentBits;
	return getBest();
}

void subseqPredictor::generateSubqueries(int* query, int size){
	int initialLength = size;
	for (int i = 0; i < size; i++){
        //cout << i << endl;
        int old_item = query[i];
        query[i] = -2;
        
        predict(query, size, MAXPREDICTIONCOUNT, initialLength, 1);
        //predict; if permanent stop abbort and return answer
        query[i] = old_item;
        //if (stop) return;
    }
    if (stop) return;
    if (size > 2){
        for (int i = 0; i < size; i++){
            for (int j = i + 1; j < size; j++){
                //cout << i << j << endl;
                int old_item_1 = query[i];
                int old_item_2 = query[j];
                query[i] = -2;
                query[j] = -2;
                
                predict(query, size, MAXPREDICTIONCOUNT, initialLength, 2);
                //predict; if permanent stop abbort and return answer

                query[i] = old_item_1;
                query[j] = old_item_2;
                //if (stop) return;
            }
        }
    }
}
