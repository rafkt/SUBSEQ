# -*- coding: utf-8 -*-
"""
    Created in Mon July 41 10:47:00 2016
    
    @author: Remi Eyraud
    
    Usage: python score_computation.py rankings_file targets_file
    Role: compute the score of a set of rankings given the target next symbols probabilities
    Example: python score_computation.py 0.spice.ranking ../targets/0.spice.target.public
"""

from sys import *
import math
import string

rankings_file = argv[1] #one ranking per line, in the same order than in the prefixes file
targets_file = argv[2] #each line for each prefix, first line for first prefix, each line: 'denominator best_symbol proba_best second_best proba_second ...'

def find_proba(letter,target):
    for i in range(len(target)):
        if target[i]==letter:
            return float(target[i+1])
    return 0

r = open(rankings_file, "r")
t = open(targets_file, "r")

score = 0
nb_prefixes = 0
for ts in t.readlines():
    nb_prefixes += 1
    rs = r.readline()
    target = string.split(ts)
    ranking = string.split(rs)

    denominator = float(target[0])
    prefix_score = 0
    k=1
    for elmnt in ranking:
        if k == 1:
            seen = [elmnt]
            p = find_proba(elmnt,target)
            prefix_score += p/math.log(k+1,2)
        elif elmnt not in seen:
            p = find_proba(elmnt,target)
            prefix_score += p/math.log(k+1,2)
            seen = seen + [elmnt]
        k += 1
        if k > 5:
           break
#print(nb_prefixes, su)
    score += prefix_score/denominator
final_score = score/nb_prefixes
print(final_score)
r.close()
t.close()
