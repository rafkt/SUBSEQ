# -*- coding: utf-8 -*-
"""
    Created in Sun April 03 14:47:00 2016
    
    @author: Remi Eyraud
    
    Installation: pip install Sp2Learning
    Usage: python spectral_baseline.py train_file prefixes_file output_file
    Role: learn a Weighted Automaton on the whole sequences of train_file, then generates a ranking of the 5 most probable symbols for each prefix of prefixes_file, stores these ranking in output_file (one ranking per line, in the same order than in the prefix file)
    Example: python spectral_baseline.py ../train/0.spice.train ../prefixes/0.spice.prefix.public 0.spice.ranking
"""
from numpy import *
from decimal import *
from sys import *
import math

train_file = argv[1] #whole sequences, PAutomaC/SPiCe format
prefixes_file = argv[2] #prefixes, PAutomaC/SPiCe format
output_file = argv[3] #to store the rankings

#set parameter values

#Estimated rank of the Hankel matrix
rank = 4

#Allow only some of the possible rows and columns of the matrix
partial = True

#Set max length of elements for rows and column
lrows = 4
lcolumns = 4

#Set which version of the matrix you want to work with
version = "factor"  # "classic" , "prefix", "suffix" , "factor"

#Set whether you want to use the sparse or the classic version of the matrix
sparse = True


def learn(train_file, parameter):
    """ Learn a weighted automaton using spectral approach
        parameter is the rank
        """
    # Import the SPiCe spectral learning toolbox
    import sp2learn.learning as LC
    from sp2learn.sample import Sample
    print ("file : ", train_file)
    # Get the learning sample in needed format
    pT = Sample(adr=train_file, lrows=lrows, lcolumns=lcolumns,
                version=version, partial=partial)

    # Create a learning instance
    S_app = LC.Learning(sample_instance=pT)
    
    # Learn an automaton (see documentation for other possible parameters)
    A = S_app.LearnAutomaton(rank=parameter, lrows=lrows,
                             lcolumns=lcolumns, version=version,
                             partial=partial, sparse=sparse)
        
    # Transform the automaton in order to compute prefix weights instead of sequence weight
    Ap = A.transformation(source="classic", target="prefix")
                             
    return Ap


def next_symbols_ranking(model, prefix, k=5):
    """ Give the sorted list of the k more frequent next symbols of the prefix in the automaton
        The model needs to compute prefix weights (and not sequence weight)
        """
    # Word has to be a list of integer (and not a string)
    # First element is the length of the prefix and thus has to be erased
    word = prefix.split()
    word = [int(i) for i in word][1:]
    
    # Compute the weight of the prefix
    p_w = model.val(word)
    for i in range(model.nbL):
        p_w -= model.val(word+[i])
    
    # Symbol -1 correspond to end of sequence
    # If the weight is negative it does not carry any semantic
    l = [(-1, max(p_w, 0))]
    s = max(p_w, 0)

    # Compute the weight of the prefix concatenated to each possible symbol
    for i in range(model.nbL):
        l.append((i, max(model.val(word+[i]), 0)))
        s += max(model.val(word+[i]), 0)

    # Sort the symbol by decreasing weight
    l = sorted(l, key=lambda x: -x[1])

    if s != 0:
    # At least one symbol has a strictly positive weight
    # Return a string containing the sorted k most probable next symbols separted by spaces
        mot = trans_string([x[0] for x in l][0:k])
        return mot
    else:
        # All symbols have a non-positive weight in the model
        # Return the k first symbols...
        return trans_string([x for x in range(-1, k-1)][0:k])

def trans_string(list):
    """ Transform a list of interger into a string of elements separated by a space """
    mot = ""
    for w in list:
        mot +=  str(w) + ' '
    return mot



def list_to_string(l):
    s=str(l[0])
    for x in l[1:]:
        s+= " " + str(x)
    return(s)


print ("Start Learning")
model = learn(train_file,rank)
print ("Learning Ended")
print("Start rankings computation")

#open prefixes and target file
p = open(prefixes_file,"r")
o = open(output_file, "w")

#get rid of first line of prefix (needed since it contains nb of example, size of the alphabet)
p.readline()

for prefix in p.readlines():
    ranking = next_symbols_ranking(model,prefix[:-1], 1)
    o.write(ranking + '\n')
print("End of rankings computation")
p.close()
o.close()
