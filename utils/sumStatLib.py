#! /usr/bin/python 

import itertools


def nucDiv(haplotypes): 
    """
    Computes the nucleotide diversity.
    """

    seqs = set(haplotypes)

    seqFreq = {}
    for seq in seqs:
        seqFreq[seq] =  haplotypes.count(seq)

    ctr =  0. 
    pi = 0. 
    for (seqA, seqB) in itertools.combinations(seqs,2):
        difs = 0. 
        for i in range(0,len(seqB)): 
            if seqA[i] != seqB[i]: 
                difs += 1.
        difs /= len(seqA)

        occA = seqFreq[seqA]
        occB = seqFreq[seqB]
        pi += difs * occA  * occB
        ctr += occA * occB 
    pi /= ctr 
    return pi 


def sfs(haplotypes): 
    """ 
    Computes the site frequency spectrum.
    """
    
    sfsDict = {}
    for i in range(0,len(haplotypes[0])): 
        tmp = reduce(lambda x,y : y + x , map(lambda x : int(x[i]), haplotypes))     
        sfsDict[tmp] = sfsDict[tmp] + 1  if sfsDict.has_key(tmp) else  1 
    return sfsDict
