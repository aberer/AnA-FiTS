#! /usr/bin/python

import sys 
import struct 

baseDict = { 
    0 : "A",
    1 : "C", 
    2 : "G", 
    3 : "T"}

typeDict= {2 : "MUT", 3 : "REC" } 

INTEGER_LENGTH = 4 
UINT8_LENGTH = 1 


def getInt():
    global content 
    global index 
    tmp = struct.unpack_from("I", content[index:index+INTEGER_LENGTH])[0]
    index += INTEGER_LENGTH
    return tmp 

def getByte(): 
    global content
    global index  
    tmp = struct.unpack_from("B", content[index:index+UINT8_LENGTH])[0]
    index += UINT8_LENGTH
    return tmp 


if len(sys.argv) != 2 : 
    print "USAGE: ./script <binary-file>"
    sys.exit()

fh = open(sys.argv[1], "rb")
content = fh.read()
index = 0; 

numGraphs =  getInt()
for graphId in xrange(0,numGraphs):
    print("// GRAPH " + str(graphId))

    # print survivors 
    sys.stdout.write("// survivingNodes ")
    numSurvivors = getInt()
    for i in xrange(0,numSurvivors): 
        sys.stdout.write(str(getInt()) + ";")
    sys.stdout.write("\n")

    numNodes = getInt() 
    # print all the nodes
    for i in xrange(0,numNodes): 
        theId=getInt()
        loc=getInt()
        originGen = getInt()
        base = baseDict[getByte()]    
        anc1Id = getInt()
        anc2Id = getInt()
        theType = getByte()
        typeStr = typeDict[theType]

        if(typeStr == "REC"): 
            print("%d\t(%d,%d)\t%d,%d" % (theId, loc, originGen, anc1Id, anc2Id))
        elif(typeStr == "MUT"): 
            print("%d\t(%d,%d,%s)\t%d" % (theId, loc, originGen, base, anc1Id))
        else : 
            sys.stderr.write("encountered unknown node") 
            sys.exit(1)

