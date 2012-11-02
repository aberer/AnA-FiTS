#! /usr/bin/python

import sys 
import random 


def helpMsg():
    print "USAGE: ./convertSeq <anafits-file> |  ./samplePopulation.py <num> <sampleIndi>\n\n\t* where <num> is the number of sequences to be sampled \n\t* and <sampleIndi> == 0, if you just want to sample random sequences \n\t  and <sampleIndi> == 1, if you want to sample the sequences of num/2 diploid individuals"
    sys.exit(1)    

if len(sys.argv) != 3 : 
    helpMsg()

chrList = [] 
fixed = []
mutations = [] 

trueNumSample = int (sys.argv[1])
numSample = int (sys.argv[1])
lines = sys.stdin.readlines()

if sys.argv[2] == "1": 
    sampleIndi = True
    numSample /= 2 
elif sys.argv[2] == "0": 
    sampleIndi = False
else : 
    helpMsg()

     

isFirst = True 
currentBv = []
for line in lines: 
    if line.startswith("// chromId"): 
        if not isFirst: 
            chrList.append(currentBv)
            currentBv = []
        isFirst = False
    elif line.startswith("// fixed"): 
        fixed.append(line.strip())
    elif line.startswith("// mutations") :
        mutations.append(line.strip())
    else :
        currentBv.append(line.strip())
        
chrList.append(currentBv) 


num = len(chrList[0])
for i in range(0,len(chrList)): 
    assert(len(chrList[i]) == num)

if(num <= numSample): 
    print("you want to sample " + str(trueNumSample) + " while there are only " + str(num) + " sequences in this file.")
    sys.exit(-1)

if(sampleIndi): 
    chosen = range(0,num / 2 )
else : 
    chosen = range(0,num)

random.shuffle(chosen)
chosen = chosen[0:numSample]

for i in range(0, len(chrList)): 
    print("// chromId " + str(i))
    print(fixed[i])
    print(mutations[i])    
    
    currentBv = chrList[i]
    
    for j in range(0, len(chosen)): 
        if(sampleIndi): 
            print currentBv[chosen[j] * 2 ]
            print currentBv[chosen[j] * 2 + 1 ]
        else : 
            print currentBv[chosen[j]]
