** SPiCe competition offline files **
* http://spice.lif.univ-mrs.fr/ *

This archive contains the files needed to use offline the data of the SPiCe competition.

The Sequence PredIction ChallengE (SPiCe) took place between April and July 2016: participants had to learn a model from a set of whole sequences and then to guess a rankings of 5 symbols that are the most likely to be the next symbol of some prefixes.

** Content of this archive **
This archive is made of 1 folders:

- 'codes' contains useful python scripts:
3gram_baseline.py that computes rankings for all prefixes of a test set given a train set using the 3-gram baseline;
spectral_baseline.py that computes rankings for all prefixes of a test set given a train set using the spectral learning baseline;
score_computation.py that computes the score given the output of the baselines (or of any program that follows that format) and a target file (the one of the 'target' directory).
Use-cases are given for each of these scripts at the beginning of the files.

**Use and cite this benchmark**
These data sets are not licensed: you are free to use them the way you want.
If you are using them in the context of a scientific paper, we would like you to cite the paper presenting the results of the competition:

@inproceedings{spice2016,
    title = {Results of the Sequence PredIction ChallengE ({SPiCe}): a competition about learning the next symbol in a sequence},
    author = {Balle Pigem, Borja and Eyraud, R{\'}emi and Luque, Franco M. and Quattoni, Ariadna and Verwer, Sicco},
    booktitle = {Proceedings of the 13th International Conference on Grammatical Inference (ICGI)},
    year = {2016},
    pages = {to appear}
}



