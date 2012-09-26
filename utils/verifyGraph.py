#! /usr/bin/python

import sys 

sys.setrecursionlimit(1000000) 

def traverse(theId, start, end):
    global encounteredMutations
    global nodeHash
    global edges 

    if(theId == 0): 
        return 
    
    node = nodeHash[theId]
    if node[3] == "MUT": 
        encounteredMutations.add(node[0])
        anc1 = filter(lambda (x,y): x == node[0], edges)
        assert(len(anc1) == 1 )         
        anc1 = anc1[0]

        if(anc1[1] != 0): 
            traverse(anc1[1], start, end)
    else : 
        ancst = filter(lambda(x,y): x == node[0],edges)
        assert(len(ancst) == 2)
        anc1 = ancst[0][1]
        anc2 = ancst[1][1]
        brkPnt = node[1]

        if anc1 != 0: 
            node1 = nodeHash[anc1]
            if(start < node[1] ):
                traverse(node1[0], start , min(end, brkPnt) )

        if anc2 != 0: 
            node2 = nodeHash[anc2]
            if(node[1] < end): 
                traverse(node2[0], max(brkPnt, start), end)    


def parseGraph(nodeHash, edges, survivingNodes):
    sys.stdin.readline()
    survivingNodes  = map(int,sys.stdin.readline().strip().split(" ")[2].strip(";").split(";"))
    lines = sys.stdin.readlines()
    for line in lines: 
        tok = line.strip().split("\t")
        theId = int(tok[0])     
        pos = int(tok[1].strip("()").split(",")[0])  
        gen = int(tok[1].strip("()").split(",")[1])  
        
        theType = "MUT"          
        anc1 = int(tok[2].strip().split(",")[0]) 
        edges.append((theId, anc1))
        if(len(tok[2].strip().split(",")) > 1): 
            theType = "REC"
            anc2 = int(tok[2].strip().split(",")[1])  
            edges.append((theId, anc2))        
        nodeHash[theId] = (theId, pos, gen, theType)
    return (nodeHash, edges, survivingNodes)

        
# hashes id to tuple 
survivingNodes = []
nodeHash = {}
edges = []

(nodeHash, edges, survivingNodes) = parseGraph(nodeHash, edges, survivingNodes)

mutations = set(map(lambda x : x[1] , filter(lambda (key,value) : value[3] == "MUT" , nodeHash.items())))
mutationIds = set(sorted(map(lambda x : x[0],  mutations)))

encounteredMutations = set()
for node in survivingNodes:     
    traverse(node, 0, 10000000)



# printing part 
encOutput =  map(lambda x : nodeHash[x] , encounteredMutations) 
for elem in encOutput: 
    print "ENC\t" + "\t".join(map(lambda x: str(x), elem))

notEncountered = mutationIds - encounteredMutations
notEncOutput = map(lambda x : nodeHash[x], notEncountered)
# print notEncOutput
for elem in  notEncOutput: 
    print "NOT\t" +  "\t".join(map(lambda x : str(x) ,  elem ))

