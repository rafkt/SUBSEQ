#  Copyright (c) 2019 Rafael Ktistakis. All rights reserved.

#
#
# ********************************
# *       backwardsSearchBWT	 *
# *        makefile              *
# ********************************
#
#
#

CC=g++ -std=c++11

FLAGS=-O3 -g -DNDEBUG -fpermissive
FLAGS_SDSL_I=-I ~/include
FLAGS_SDSL_L=-L ~/lib
OFLAGS=-c
LIBS=-lsdsl -ldivsufsort -ldivsufsort64

EXEFLAGS=-o ./program
EXEFLAGS_KF=-o ./k_fold_run
EXEFLAGS_TST=-o ./test_validation

	REM=rm -rf
	FND_FILES=find . -type f -name
	REM_FILES=-exec rm {} \;
	XF=.exe
	IN=.o

all: main 
#
#all: test k_fold

test: testingMain.cpp LRUcache.o
	$(CC) $(FLAGS) LRUcache.o testingMain.cpp $(EXEFLAGS_TST)

k_fold: k-folder_evaluator_run.cpp backwardsSearchBWT.o suffixArray.o predictor.o similar_queries.o LRUcache.o k_folder.o csvProducer.o
	$(CC) $(FLAGS) $(FLAGS_SDSL_I) $(FLAGS_SDSL_L) suffixArray.o similar_queries.o backwardsSearchBWT.o predictor.o LRUcache.o csvProducer.o k_folder.o k-folder_evaluator_run.cpp $(EXEFLAGS_KF) $(LIBS)

main: main.cpp backwardsSearchBWT.o suffixArray.o subseqPredictor.o
	$(CC) $(FLAGS) $(FLAGS_SDSL_I) $(FLAGS_SDSL_L) suffixArray.o backwardsSearchBWT.o subseqPredictor.o main.cpp $(EXEFLAGS) $(LIBS)

backwardsSearchBWT.o: backwardsSearchBWT.cpp backwardsSearchBWT.h
	$(CC) $(OFLAGS) $(FLAGS) $(FLAGS_SDSL_I) backwardsSearchBWT.cpp

suffixArray.o: suffixArray.cpp suffixArray.h
	$(CC) $(OFLAGS) $(FLAGS) $(FLAGS_SDSL_I) suffixArray.cpp

similar_queries.o: similar_queries.cpp similar_queries.h
	$(CC) $(OFLAGS) $(FLAGS) similar_queries.cpp

predictor.o: predictor.cpp predictor.h
	$(CC) $(OFLAGS) $(FLAGS) $(FLAGS_SDSL_I) predictor.cpp

subseqPredictor.o: subseqPredictor.cpp subseqPredictor.h
	$(CC) $(OFLAGS) $(FLAGS) $(FLAGS_SDSL_I) subseqPredictor.cpp

k_folder.o: k_folder.cpp k_folder.h LRUcache.o
	$(CC) $(OFLAGS) $(FLAGS) k_folder.cpp LRUcache.o

LRUcache.o: LRUcache.cpp LRUcache.h
	$(CC) $(OFLAGS) $(FLAGS) LRUcache.cpp

csvProducer.o: csvProducer.cpp csvProducer.h
	$(CC) $(OFLAGS) $(FLAGS) csvProducer.cpp

clean:
	$(REM) *$(XF)
	$(REM) *$(IN)
	$(REM) program sdb.sdsl program.dSYM
	$(FND_FILES) trainingFile\* $(REM_FILES)
