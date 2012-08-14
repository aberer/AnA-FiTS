#pragma once 

#include "common.hpp"
#include "NodeManager.hpp"
#include "AddrArray.hpp"
#include "Node.hpp"
#include "SequenceArray.hpp"

#include <fstream>
#include <set>
#include <iostream>
#include <vector>

class Ancestry; 
class Randomness;
class Chromosome; 
class Survivors; 
class PopulationManager; 


class Graph
{
public: 
  // LIFE CYCLE
  explicit Graph(nat initSize);
  
  // OPERATORS
  void touchNode(seqLen_t loc, nat indiNr, nat genNum, NodeType type ); 
  void hookup(const Survivors &survivors,  const Ancestry &ancestry, const PopulationManager &popMan, Chromosome &chrom, Randomness &rng, nat startGen, nat endGen); 
  void createSequencesInGraph(const Chromosome &chromo); 

  // OBSERVERS
  nat getNumberOfMutations() {return mutNodes.getNumberUsed(); }
  vector<Node*>& getState() {return previousState; }
  NeutralArray* getSequenceFromNode(Node *node){ return nodMan.getInfo(node->id)->sequence; }
  void printRaw(FILE *fh);

#ifdef VERIFICATION
  void getSequencesSlow(vector<NeutralArray*> &seqs, Chromosome &chrom, Randomness &rng);
#endif

private: 
  // ATTRIBUTES  
  AddrArray<Node,true> mutNodes; 
  AddrArray<Node,true> recNodes;
  vector<Node*> previousState; 
  NodeManager nodMan; 
  DynArraySequential<Node*> buffer; 

  // METHODS
  Graph& operator=(const Graph &rhs); 
  Graph(const Graph& rhs);
  
  void propagateSurvivorNodes(nat genC, nat chromId, Node **nowBuffer, Node **prevBuffer, const Ancestry &ancestry, const Survivors &survivors);
  void insertMutEvents(nat genC, AddrArrayBackwardIter<Node,true> &mutIter, Node** nodeBufferNowGen, Chromosome &chrom, Randomness &rng);
  void insertRecEvents(nat genC, nat chromId, AddrArrayBackwardIter<Node,true> &recIter, Node** nodeBufferNowGen, Node** nodeBufferPrevGen, const Ancestry &ancestry);
  Node* hookRecombinations(Node *anc1, Node *anc2);
#ifdef VERIFICATION
  void gatherSequencesSlow(Node *node, NeutralArray *seq, seqLen_t start, seqLen_t end, Chromosome &chrom, Randomness &rng);
#endif
};


////////////////////
// IMPLEMENTATION //
////////////////////
