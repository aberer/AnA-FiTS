#pragma once 

#include "common.hpp"
#include "ProgramOptions.hpp"
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

#define GET_ID_IF(x) (x ? x->id : NO_NODE) 


class Graph
{
public: 
  // LIFE CYCLE
  explicit Graph(nat initSize, const ProgramOptions &progOpt );

  // OPERATORS
  void touchNode(seqLen_t loc, nat indiNr, nat genNum, NodeType type , bool invertedOrientation); 
  void hookup(const Survivors &survivors,  const Ancestry &ancestry, const PopulationManager &popMan, Chromosome &chrom, Randomness &rng, nat startGen, nat endGen); 
  void createSequencesInGraph(const Chromosome &chromo); 
  void rectifyMultipleNP(Randomness &rng);

  // OBSERVERS
  nat getNumberOfMutations() {return mutNodes.getNumberUsed(); }
  vector<Node*>& getState() {return previousState; }
  void getRawSequences(vector<BitSetSeq*> &bvs); 
  vector<Node*>& getBvMeaning(){return nodMan.getBvMeaning();} // bad style =( 
  BitSetSeq* getBvFromNode(Node *node){return nodMan.getInfo(node->id)->bv; }
  void printRaw(FILE *fh) ;

private: 
  // ATTRIBUTES  
  AddrArray<Node,true> mutNodes; 
  AddrArray<Node,true> recNodes;
  vector<Node*> previousState; 
  NodeManager nodMan; 
  DynArraySequential<Node*> buffer; 

#ifndef NDEBUG
  bool survivorsContainStartingNode; 
#endif

  // METHODS
  Graph& operator=(const Graph &rhs); 
  Graph(const Graph& rhs);
  
  void propagateSurvivorNodes(nat genC, nat chromId, Node **nowBuffer, Node **prevBuffer, const Ancestry &ancestry, const Survivors &survivors);
  void insertMutEvents(nat genC, AddrArrayBackwardIter<Node,true> &mutIter, Node** nodeBufferNowGen, Chromosome &chrom, Randomness &rng);
  void insertRecEvents(nat genC, nat chromId, AddrArrayBackwardIter<Node,true> &recIter, Node** nodeBufferNowGen, Node** nodeBufferPrevGen, const Ancestry &ancestry);
  Node* hookRecombinations(Node *anc1, Node *anc2);
};


////////////////////
// IMPLEMENTATION //
////////////////////
