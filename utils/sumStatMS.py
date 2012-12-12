#! /usr/bin/python

import sys 
import sumStatLib
from progressbar import * 

USAGE="Please the pipe output of ms into this program."

if sys.stdin.isatty() :
    print USAGE 
    sys.exit(1)

seqLen = 100000 



def oneSample(numSamp): 
    line = sys.stdin.readline().strip()
    assert(line == "")

    line = sys.stdin.readline().strip()
    assert(line == "//")

    numSnp = int(sys.stdin.readline().strip().split(" " )[1])
    line = sys.stdin.readline()
    
    haplotypes = []
    for i in range(0,numSamp): 
        haplotypes.append(sys.stdin.readline().strip()) 
    return haplotypes
        


line = sys.stdin.readline()
tok = line.split(" ")
numSamp = int(tok[1])
numRep = int(tok[2]) 
sys.stdin.readline()
ctr = 0

widgets = ['progress: ', Percentage(), ' ', Bar(marker='=',left='[',right=']'),
           ' ', ETA()] 
pbar = ProgressBar(widgets=widgets, maxval=numRep-1)
pbar.start()

numSnp = []
numHap = []
pies = []
sfsDict = {}

for i in range(0,numRep-1): 
    pbar.update(i)
    haplotypes = oneSample(numSamp)
    
    numSnp.append(len(haplotypes[0]) ) 
    numHap.append(len(set(haplotypes)))
    pies.append(sumStatLib.nucDiv(haplotypes, seqLen)) # :TRICKY:
    
    tmpDict = sumStatLib.sfs(haplotypes)
    ctr += 1 

    for key in tmpDict: 
        if sfsDict.has_key(key): 
            sfsDict[key] += float(tmpDict[key])
        else : 
            sfsDict[key] = float(tmpDict[key])
pbar.finish()

    
for key in sfsDict.keys():
    sfsDict[key] /= float(numRep)

assert(ctr == numRep-1)


fh = open("numHap.txt", "w")
for elem in numHap: 
    fh.write("%s\n" % elem)

fh = open("numSnp.txt", "w")
for elem in numSnp: 
    fh.write("%s\n" % elem)

fh = open("pi.txt", "w")
for elem in pies: 
    fh.write("%s\n" % elem)

fh = open('sfs.txt', 'w')
for key in sfsDict.keys(): 
    fh.write("%d\t%f\n" % (key ,sfsDict[key]) )
fh.close()
