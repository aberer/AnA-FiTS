#pragma once 

#include "types.hpp"
#include "common.hpp"
#include "Node.hpp"
#include "DynArraySequential.hpp"

#include <list>
#include <iostream>

/**
 * Handles id stuff and manages the extra info. Thus, we can avoid
 * storing a lot of extra information within the Node's, but can throw
 * it away, once we do not need it any more.
 */


class NodeManager
{
public: 
  // LIFE CYCLE 
  explicit NodeManager(nat initSize);
  ~NodeManager();
  
  // OPERATORS  
  void registerNextNode(Node &node); 
  void markAncestrialMaterial(Node &node, seqLen_t start, seqLen_t end);  
  Node* getNode(nat id){assert(id == 0 ||  relevantNodes[id]->id ==id) ; return  relevantNodes[id];} 
  NodeExtraInfo* getInfo(nat id) { return &(extraInfo[id]); }
  void checkConsistency();
  void determineCoalescentNodes(Node *node);   
  nat getNumberOfNodesUsed(){return highestId; }
  void simulateNode(Node *node);   

private:
  Node** relevantNodes; 
  NodeExtraInfo* extraInfo;
  nat length; 
  nat highestId; 
  DynArraySequential<NeutralArray*> allocatedSeqs; // just for freeing them later 
  DynArraySequential<NeutralMutation*> allocatedMuts; 

  nat getTrueAncestor(Node *node); 
  void resize();
  void handleAncestor(NeutralArray *seq, Node *anc , seqLen_t start, seqLen_t end); 
  void gatherMutations(Node *node, NeutralArray* seq, seqLen_t start, seqLen_t end);
  NeutralMutation* createNeutralMutation(Node *node);
};
