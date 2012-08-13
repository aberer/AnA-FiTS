#! /usr/bin/python

from bitarray import bitarray  
import random
import sys

if ( len(sys.argv) != 2):
    print "need input file "
    sys.exit()


fh = open(sys.argv[1])

# throw away first three lines
fh.readline()
fh.readline()
fh.readline()
lines = fh.readlines()

bitsets = []
for line in lines:
    bs = bitarray(line.strip().split("\t")[1])
    bitsets.append(bs)

for i in range(0,10000):
    seqA = random.choice(bitsets)
    seqB = random.choice(bitsets)
    
    differences = seqA ^ seqB 
    print differences.count()
