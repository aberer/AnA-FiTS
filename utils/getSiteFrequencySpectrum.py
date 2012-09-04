#! /usr/bin/python

import sys



USAGE="pipe the output of utils/convertSeq.py into this script\n (e.g. ./utils/converSeq.py anafits_polymorphisms.<id> | ./utils/getSiteFrequencySpectrum.py )"

if not sys.stdin.isatty():
    lines = sys.stdin.readlines()
else : 
    print USAGE
    sys.exit(1)

if len(lines) == 0: 
    print USAGE
    sys.exit(1)

seqs = []
lines = lines[3:]

numInSeq = 0
for line in lines:
    if not  line.startswith("//"): 
        seq = line.strip().split("\t")[1]
        seqs.append(seq)
        numInSeq = len(seq) 



sfs = []
for i in range(0,numInSeq):     
    num = 0 
    for j in range(0, len(seqs)): 
        seq = seqs[j]
        assert(len(seq) == numInSeq)
        if (seqs[j][i] == '1' ) : 
            num += 1 
    sfs.append(num)

for i in range(0,len(sfs)): 
    print sfs[i]


        
        
