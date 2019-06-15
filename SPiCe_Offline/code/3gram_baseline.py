# -*- coding: utf-8 -*-
"""
    Created in Thu March  22 10:47:00 2016
    
    @author: Remi Eyraud & Sicco Verwer
    
    Usage: python 3gram_baseline.py train_file prefixes_file output_file
    Role: learn a 3-gram on the whole sequences of train_file, then generates a ranking of the 5 most probable symbols for each prefix of prefixes_file, stores these ranking in output_file (one ranking per line, in the same order than in the prefix file)
    Example: python 3gram_baseline.py ../train/0.spice.train ../prefixes/0.spice.prefix.public 0.spice.ranking
"""




from numpy import *
from decimal import *
from sys import *
import math

train_file = argv[1] #whole sequences, PAutomaC/SPiCe format
prefixes_file = argv[2] #all prefixes, PAutomaC/SPiCe format
output_file = argv[3] #to store the rankings on the prefixes


def number(arg):
    return Decimal(arg)

def threegramdict(sett):
 DPdict = dict()
 total = 0
 for sequence in sett:
     ngramseq = [-1,-1] + sequence + [-2]
     for start in range(len(ngramseq)-2):
         total
         end = start + 2
         if tuple(ngramseq[start:end]) in DPdict:
             table = DPdict[tuple(ngramseq[start:end])]
             if ngramseq[end] in table:
                 table[ngramseq[end]] = table[ngramseq[end]] + 1
             else:
                 table[ngramseq[end]] = 1
             table[-1] = table[-1] + 1
         else:
             table = dict()
             table[ngramseq[end]] = 1
             table[-1] = 1
             DPdict[tuple(ngramseq[start:end])] = table
 return DPdict

def threegramrank(prefix, alphabet, DPdict):
    probs=[]
    # Compute the probability for prefix to be a whole sequence
    prob = number('1.0')
    ngramseq = [-1,-1] + prefix + [-2]
    for start in range(len(ngramseq)-2):
        end = start + 2
        if tuple(ngramseq[start:end]) in DPdict and ngramseq[end] in DPdict[tuple(ngramseq[start:end])]:
            prob = prob * (number(DPdict[tuple(ngramseq[start:end])][ngramseq[end]]) / number(DPdict[tuple(ngramseq[start:end])][-1]))
        else:
            # Substring not in the dictionnary
            prob = number(0)
    probs.append((-1,prob))
    for x in range(alphabet):
        prob = number('1.0')
        ngramseq = [-1,-1] + prefix + [x]
        for start in range(len(ngramseq)-2):
            end = start + 2
            if tuple(ngramseq[start:end]) in DPdict and ngramseq[end] in DPdict[tuple(ngramseq[start:end])]:
                prob = prob * (number(DPdict[tuple(ngramseq[start:end])][ngramseq[end]]) / number(DPdict[tuple(ngramseq[start:end])][-1]))
            else:
                # Substring not in the dictionnary
                prob=number(0)
        probs.append((x,prob))
    probs=sorted(probs, key=lambda x: -x[1])
    return [x[0] for x in probs]



def readset(f):
 sett = []
 line = f.readline()
 l = line.strip().split(" ")
 num_strings = int(l[0])
 alphabet_size = int(l[1])
 for n in range(num_strings):
     line = f.readline()
     l = line.strip().split(" ")
     sett = sett + [[int(i) for i in l[1:len(l)]]]
 return alphabet_size, sett


def list_to_string(l):
    s=str(l[0])
    for x in l[1:]:
        s+= " " + str(x)
    return(s)

print("Getting training sample")
alphabet, train = readset(open(train_file,"r"))
print ("Start Learning")
dict=threegramdict(train)
print ("Learning Ended")

print("Start rankings computation")
#open prefixes
p = open(prefixes_file,"r")
o = open(output_file, "w")
#get rid of first line of prefixes_file (needed since it contains nb of example, size of the alphabet)
p.readline()

for prefix in p.readlines():
    pl = prefix.split()
    pi = [int(i) for i in pl[1:len(pl)]]
    ranking = threegramrank(pi, alphabet, dict)[:5]
    for i in range(len(ranking)):
        o.write(str(ranking[i])+ ' ')
    o.write('\n')

print("End of rankings computation")
p.close()
o.close()


