#! /usr/bin/python

# general pattern: 
#  * # + mutations
#  * # + fixed mutations
#  * # + bitsets


import sys
import struct

INTEGER_LENGTH = 4 
BYTE_LENGTH=1
FLOAT_LENGTH=8                  # currently using doubles 
BITSET_UNIT=32

allBits = 1
for i in range(1,32) : 
    allBits = (allBits << 1)  + 1 

index = 0
table = [1]

baseDict = { 
    0 : "A",
    1 : "C", 
    2 : "G", 
    3 : "T"}


def printMutation(mut):
    if(mut[2] == 0): 
        sys.stdout.write("%d,%d,%.0f,%s;" % mut)
    else: 
        sys.stdout.write("%d,%d,%.5f,%s;" % mut)


def getBitset(intLength):
    global index 
    global content 
    bs = struct.unpack_from(bsString, content[index:(index + (intLength * INTEGER_LENGTH))])
    index += (intLength * INTEGER_LENGTH)
    return bs

def getInt(): 
    global index 
    global content
    tmp = struct.unpack_from("I", content[index:(index+INTEGER_LENGTH)])[0]
    index += INTEGER_LENGTH
    return tmp 

def getMut():
    global index 
    global content 
    tmp = struct.unpack_from("I I d B", content[index:(index+INTEGER_LENGTH + INTEGER_LENGTH + FLOAT_LENGTH + BYTE_LENGTH)]) 
    index += INTEGER_LENGTH + INTEGER_LENGTH + FLOAT_LENGTH + BYTE_LENGTH
    return (tmp[0], tmp[1], tmp[2], baseDict[tmp[3]]) 

def test(bitset, num):
    res = (bitset[num / BITSET_UNIT] & table[num % BITSET_UNIT]) != 0
    if(res): 
        return "1"
    else :    
        return "0"

def andify(target, src,num):
    for i in range(0,num):
        target[i] = target[i] & src[i]
    return target


def printChromosome(chromId, fixedMutations, mutations, bitsets, fixedNeutral):
    print "// chromId " + str(chromId)

    # add neutral fixed to fixed 
    for i in range(0,len(mutations)):
        if(test(fixedNeutral,i) == "1"):
            fixedMutations.append(mutations[i])

    # print it     
    sys.stdout.write("fixed:\t")
    if(len(fixedMutations) == 0):
        sys.stdout.write("none")
    else :
        for fixedMutation in fixedMutations:             
            printMutation(fixedMutation)
    sys.stdout.write("\n")

    sys.stdout.write("mutations:\t")
    c = 0
    for mutation in mutations: 
        if test(fixedNeutral,c) == "0" :
            printMutation(mutation)
        c += 1
    sys.stdout.write("\n")

    c = 0
    for bs in bitsets:
        string = ""
        for i in range(0,numMut):
            if test(fixedNeutral,i) == "0" :
                string += test(bs,i)
        print str(c)+ "\t" +  string
        c += 1

    
if len(sys.argv) != 2: 
    print "./script <dataFile>"
    sys.exit(1)

fh = open(sys.argv[1], "rb")
content = fh.read()
index = 0; 

# init table 
for i in range(1,32): 
    table.append(table[i-1] * 2)


numChrom = getInt()
for chromId in range(0,numChrom):    
    # get fixed mutations
    fixedMutations = []
    numFixed = getInt()
    for i in xrange(0,numFixed):
        fixedMutations.append(getMut())

    # get mutations
    mutations = []
    numMut = getInt()
    for i in xrange(0,numMut):
        mutations.append(getMut())    

    # get bitsets 
    bitsets = []        
    bytesInBitset = len(mutations) / BITSET_UNIT
    elemsInBitset = len(mutations)
    if( BITSET_UNIT * bytesInBitset <  len(mutations)):
        bytesInBitset += 1
    bsString = ""
    for  i in range(0,bytesInBitset):
        bsString += " I" 

    numSeq = getInt()
    for i in range(0,numSeq):     
        bitsets.append(getBitset(bytesInBitset))
    assert(len(bitsets) == numSeq)        

    # determine fixed neutral mutations
    fixedNeutral = []
    for i in range(0,bytesInBitset): 
        fixedNeutral.append(allBits)
    for bs in bitsets: 
        fixedNeutral = andify(fixedNeutral, bs, bytesInBitset)
    
    printChromosome(chromId, fixedMutations, mutations, bitsets, fixedNeutral)
