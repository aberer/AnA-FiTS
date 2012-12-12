#! /usr/bin/python

import sys 
import random 
import sumStatLib

seqLen = 100000 


USAGE="""pipe output of convertSeq into this program.
e.g., 
./convertSeq anafits_polymorphisms.<id> | ./statistics.py

This script currently does not accept more than 1 locus. 
"""

if not sys.stdin.isatty() or len(sys.argv) != 2 :
    lines = sys.stdin.readlines()
else : 
    print USAGE
    sys.exit(1)

if len(filter(lambda x : x.startswith("// chrom") ,  lines)) > 1 : 
    print USAGE
    sys.exit(1)

numSample =  int(sys.argv[1]) 

lines = filter(lambda x : not x.startswith("// chrom"), lines)

# omitting mutation info 
mutations = lines[1].strip().split("\t")[1].split(";")
mutations = filter(lambda x : not x == '' , mutations)
mutations = map(lambda x : int(x.split(",")[0]) , mutations)

# assert
sortedMutations = mutations
sortedMutations.sort()
for i in range(0, len(mutations)): 
    assert(mutations[i] == sortedMutations[i])

haplotypes = map(lambda x : x.strip().split("\t")[1], lines[2:len(lines)])

random.shuffle(haplotypes)
haplotypes = haplotypes[0:numSample]

newHap = []
for i in range(0,numSample): 
    newHap.append([])

# print "numMut\t" + str(len(mutations))

for i in range(0,len(mutations)): 
    bla = map(lambda x : x[i], haplotypes)
    occ = reduce(lambda x,y: int(x) + int(y) , bla )
    if not (occ == 0 or occ == numSample): 
        for j in range(0,len(haplotypes)) : 
            newHap[j].append(bla[j])

haplotypes = []
for h in newHap: 
    haplotypes.append( "".join(h)  )     

print "#snp\t%d"  %  len(haplotypes[0])
print "#h\t%d"  % len(set(map(lambda x : "".join(x), haplotypes))) 

pi = sumStatLib.nucDiv(haplotypes, seqLen ) # :TRICKY: 
print "pi\t%f" % pi 

print "sfs"
sfsDict =  sumStatLib.sfs(haplotypes)
for key in sfsDict.keys():
    print "%d\t%d" % (key, sfsDict[key])
