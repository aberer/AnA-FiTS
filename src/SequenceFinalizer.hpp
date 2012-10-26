#pragma once 

#include "common.hpp"
#include "BitSet.hpp"
#include "Node.hpp"

#include <vector>



class SequenceFinalizer
{
public: 
  SequenceFinalizer();
  ~SequenceFinalizer(); 
  
  // operators 
  void computeFinalSequences(vector<BitSetSeq*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences, SelectedArray *fixedSelected);
  
  // observers 
  vector<BitSetSeq*> getFinalSequences() {return finalSequences; } 
  vector<SelectedMutation*> getMutations() {return mutationsInSeqs; }
  vector<SelectedMutation*> getFixedMutations(){ return fixedMutations; }
  void printBinary(FILE *fh);
  
private: 
  vector<BitSetSeq*> finalSequences;
  vector<SelectedMutation*> mutationsInSeqs;
  vector<SelectedMutation*> fixedMutations;
  
  void annotateFixedMutations(SelectedArray *fixedMutations, BitSetSeq &fixedNeutral, vector<Node*> &bvMeaning); 
  void annotateRelevantMutations(BitSetSeq &notPresent, BitSetSeq &fixedRawMut, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences); 
  void createMergedBitvectors(BitSetSeq &notPresent, BitSetSeq &fixedRawMut, vector<BitSetSeq*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences); 
};


bool operator==(SelectedMutation &lhs, SelectedMutation &rhs); 
bool operator==(SelectedMutation &lhs, Node &rhs);   
bool operator==(Node &lhs, SelectedMutation &rhs); 
