#! /usr/bin/python

import sys
try: 
    import networkx as nx 
    import pygraphviz as gv
except: 
    sys.stderr.write("this script requires the following packages: pygraphviz, networkx\nPlease install and try again\n")

import re 

USAGE=" <OUTPUT from ./convertGraph.py >  | ./utils/visualizeGraph.py "


num="[0-9]+"
tab="\t"
base="[ATCG]"


mutPattern = re.compile("(%s)\t\((%s),(%s),(%s)\)\t(%s)" % (num,num,num,base,num))
recPattern = re.compile("(%s)\t\((%s),(%s)\)\t(%s),(%s)" % (num,num,num,num, num))

if not sys.stdin.isatty():
    lines = sys.stdin.readlines()
else : 
    print USAGE
    sys.exit(1);

if(len(lines) == 0): 
    print USAGE
    sys.exit(1)


class RecNode:
    def __init__(self, nodeId, pos,gen, ancst1, ancst2): 
        self.nodeId = nodeId        
        self.pos = pos
        self.gen = gen
        self.ancst1  = ancst1
        self.ancst2 = ancst2

    def repr(self): 
        return ("<%d,%d,%d,%d,%d>" % (self.nodeId, self.pos, self.gen, self.ancst1, self.ancst2))
    def str(self): 
        return self.repr()

class MutNode : 
    def __init__(self, nodeId, pos, gen, base, ancst): 
        self.nodeId = nodeId 
        self.pos = pos
        self.gen = gen
        self.base = base 
        self.ancst = ancst    

    def repr(self): 
        return ("<%d,%d,%d,%s,%d>" % (self.nodeId, self.pos, self.gen, self.base, self.ancst))
    def str(self): 
        return self.repr()

    
class MyGraph:
    survivorIds = []
    allNodes = {}
    graph = nx.DiGraph()

    def __init__(self):
        dummy = MutNode(0,0,0,'A', 0)
        self.graph.add_node(dummy)
        self.allNodes[0] = dummy

    def getNodesFromToGen(self, start,end): 
        result = []
        highestId = max(self.allNodes.keys())
        for i in range(1, highestId): 
            assert(self.allNodes.has_key(i))
            node = self.allNodes[i]
            if node.gen <= end and node.gen >= start : 
                result.append(node)
        return nx.subgraph(self.graph, result)


    def insertEdges(self, edgeList): 
        for elem in edgeList: 
            print elem
            # print repr(self.allNodes[elem[0]]) + "\t" + repr(self.allNodes[elem[1]])
            self.graph.add_edge(self.allNodes[elem[0]], self.allNodes[elem[1]])


def parse(lines): 
    graphs = []     
    
    assert(lines[0].startswith("// GRAPH"))
    isFirst = True 
    edgeList = []


    for i in range(0,len(lines)): 
        line = lines[i].strip()
        if(line.startswith("// GRAPH")):
            if not isFirst: 
                graph.insertEdges(edgeList)
                edgeList = []
                graphs.append(graph)
                graph = MyGraph() 
            else : 
                graph = MyGraph()
                isFirst = False
            continue 
            
        if(line.startswith("// surviving")): 
            graph.survivorIds = line.split(" " )[2].strip(";").split(";") 
            continue

        mr = recPattern.match(line)
        mm = mutPattern.match(line)
        assert(not (mr ==   None and  mm  ==  None))
        if(not mr == None ): 
            rn = RecNode(int(mr.group(1)), int(mr.group(2)), int(mr.group(3)), int(mr.group(4)), int(mr.group(5)))
            nodeId = int(mr.group(1))
            assert(not graph.allNodes.has_key(nodeId))
            graph.allNodes[nodeId] = rn
            graph.graph.add_node(rn)
            edgeList.append((nodeId, rn.ancst1) )
            edgeList.append((nodeId, rn.ancst2))


        elif(not mm == None ):             
            mn = MutNode(int(mm.group(1)), int(mm.group(2)), int(mm.group(3)), mm.group(4), int(mm.group(5))) 
            nodeId = int(mm.group(1))
            assert(not graph.allNodes.has_key(nodeId))            
            graph.allNodes[nodeId] = mn 
            graph.graph.add_node(mn) 
            edgeList.append((nodeId,mn.ancst))

    graph.insertEdges(edgeList)
    graphs.append(graph)

    return graphs

graphs = parse(lines)

graph = graphs[0]
subgraph = graph.getNodesFromToGen(0, 100)


import matplotlib.pyplot as plt
plt.figure()
nx.draw(subgraph)
plt.savefig("tmp.pdf")

# vizGraph = nx.to_agraph(subgraph)
# vizGraph.layout(prog='dot')
# vizGraph.draw('tmp.pdf')
