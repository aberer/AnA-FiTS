#! /usr/bin/python

import sys 
import random 

if len(sys.argv) != 2 : 
    print "USAGE: ./convertSeq <anafits-file> |  ./samplePopulation.py <num>"
    sys.exit(1)

chrList = [] 
fixed = []
mutations = [] 

numSample = int (sys.argv[1])
lines = sys.stdin.readlines()

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
    print("you want to sample " + str(numSample) + " while there are only " + str(num) + " sequences in this file.")
    sys.exit(-1)

chosen = range(0,num)
random.shuffle(chosen)
chosen = chosen[0:numSample]

for i in range(0, len(chrList)): 
    print("// chromId " + str(i))
    print(fixed[i])
    print(mutations[i])    
    
    currentBv = chrList[i]
    
    for j in range(0, len(chosen)): 
        print currentBv[chosen[j]]
