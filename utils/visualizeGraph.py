#! /usr/bin/python

import sys
import networkx as nx 
import pygraphviz as gv
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

class MutNode : 
    def __init__(self, nodeId, pos, gen, base, ancst): 
        self.nodeId = nodeId 
        self.pos = pos
        self.gen = gen
        self.base = base 
        self.ancst = ancst    

    
class MyGraph:
    survivorIds = []
    allNodes = {}
    graph = nx.DiGraph()

    def getNodesFromToGen(self, start,end): 
        result = []
        highestId = max(self.allNodes.keys())
        for i in range(1, highestId): 
            assert(self.allNodes.has_key(i))
            node = self.allNodes[i]
            if node.gen <= end and node.gen >= start : 
                result.append(node)
        return nx.subgraph(self.graph, result)


def parse(lines): 
    graphs = []     
    
    assert(lines[0].startswith("// GRAPH"))
    isFirst = True 

    for i in range(0,len(lines)): 
        line = lines[i].strip()
        if(line.startswith("// GRAPH")):
            if not isFirst: 
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

            # TODO 
            # graph.graph.add_edge(nodeId, rn.ancst1)
            # graph.graph.add_edge(nodeId, rn.ancst2)



        elif(not mm == None ):             
            mn = MutNode(int(mm.group(1)), int(mm.group(2)), int(mm.group(3)), mm.group(4), int(mm.group(5)))            
            nodeId = int(mm.group(1))
            assert(not graph.allNodes.has_key(nodeId))            
            graph.allNodes[nodeId] = mn 
            graph.graph.add_node(mn ) 

            # TODO 
            graph.graph.add_edge(nodeId, mn.ancst)

    graphs.append(graph)

    return graphs

graphs = parse(lines)

graph = graphs[0]
subgraph = graph.getNodesFromToGen(0, 10000)

vizGraph = nx.to_agraph(subgraph)
vizGraph.layout(prog='dot')
vizGraph.draw('tmp.pdf')
