#pragma once 

#include "types.hpp"
#include "common.hpp"
#include "BitSet.hpp"
#include "Node.hpp"
#include "DynArraySequential.hpp"

#include <list>
#include <vector>
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
  void getCoalStatistic(); 
  nat findInBv(seqLen_t key ) const; 
  
  void markAncestrialMaterial(Node &node, seqLen_t start, seqLen_t end);  
  Node* getNode(nat id){assert(id == 0 ||  relevantNodes[id]->id ==id) ; return  relevantNodes[id];} 
  NodeExtraInfo* getInfo(nat id) { return &(extraInfo[id]); }
  void checkConsistency();
  void determineCoalescentNodes(Node *node);   
  void setStartNode(Node *node);

  // OBSERVERS 
  vector<Node*>& getBvMeaning(){return bvMeaning;}
  nat getNumberOfNodesUsed(){return highestId; }
  
  void initBvMeaning();
  void createSequenceForNode(Node *node); 

private:
  Node** relevantNodes; 
  NodeExtraInfo* extraInfo;
  nat length; 
  nat highestId; 
  DynArraySequential<NeutralArray*> allocatedSeqs; // just for freeing them later 
  DynArraySequential<NeutralMutation*> allocatedMuts; 
  DynArraySequential<BitSet<uint64_t>*> allocatedBvs; 
  vector<Node*> bvMeaning;   

  void handleAncestorBv(BitSet<uint64_t> *bv, Node *node, seqLen_t start, seqLen_t end); 
  void accumulateMutationsBv(Node *node, BitSet<uint64_t> *bv, seqLen_t start, seqLen_t end); 
  nat getStartIdx(seqLen_t start); 
  nat getEndIdx(seqLen_t start); 
  nat getTrueAncestor(Node *node); 
  void resize();
  NeutralMutation* createNeutralMutation(Node *node);
};

