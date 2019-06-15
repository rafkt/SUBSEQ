# SUBSEQ
Succinct BWT-based Sequence prediction

Evaluation environment, data used, experimental data as published in DEXA 2019


## Running instructions:

### in IPredict/outputs:

Delete (if any) old outputs files with:
parallel --jobs 1 rm -rf  {1}*fold*.{2}.txt ::: MSNBC FIFA KOSARAK BIBLE_CHAR BIBLE_WORD SIGN BMS ::: queries training consequent

Run IPredict to get accuracy results for lossy predictors and CPT/CPT+ and export their answers/consequents.

### Then in SUBSEQ:

parallel --jobs 1 ./program ../../Ipredict/outputs/{2}.fold.{1}.training.txt ../../Ipredict/outputs/{2}.fold.{1}.queries.txt ">>" ../../Ipredict/outputs/sbp/{2}.fold.{1}.answers.sbp.txt ::: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 ::: BMS SIGN MSNBC BIBLE_WORD BIBLE_CHAR FIFA KOSARAK

The above will export answers from SUBSEQ

### In IPredict/outputs/sBP
parallel --jobs 1 java answerValidator {2} {1} ::: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 ::: BMS SIGN MSNBC BIBLE_WORD BIBLE_CHAR FIFA KOSARAK

The above will calculate the accuracy rate of SUBSEQ or SPiCe

(addapt/tweak-code accordingly for different datasets/folds and for checking the consequents with different Predictors answers e.g SPiCe or SUBSEQ)

The current IPredict framework is tweaked for (1) reading SPiCe data (2) map and export data symbols in a range of (0,sigma] (3) print-out useful evaluations metrics (4) export training/testing/evaluation data.

The original IPredict can be found on: https://github.com/tedgueniche/IPredict

### In IPredict/outputs
SPiCe baseline reads training/testing data in a different form structure; useful script to run and prepare IPredict/datasets for SPiCe:

parallel -j 1 python convert_for_spice.py {1} {2} {3} ::: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 ::: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 ::: "training" "queries"

(tweak according to input data that are due for conversion)

### In SPiCe_Offline:

Run spectral_baseline.py and export answers for calculating accuracy rate with answerValidator
More instructions for Spice can be found on: http://spice.lif.univ-mrs.fr


