#! /usr/bin/python

import random
import sys 

if ( len(sys.argv) != 2):
    print "need input file "
    sys.exit()


fh = open(sys.argv[1])
lines = fh.readlines()

seqs = []
for line in lines:
    seq = set(line.strip().split(";"))
    seqs.append(seq)


for i in range(0,1000) :
    seqA = random.choice(seqs)
    seqB = random.choice(seqs)
    
    print("%d\t%d\t%d\t%d" % ( len(seqA), len(seqB), len(seqA - seqB), len(seqA & seqB))) 
