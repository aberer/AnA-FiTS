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
  void computeFinalSequences(vector<BitSet<uint64_t>*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences, SelectedArray *fixedSelected);
  vector<BitSet<uint64_t>*> getFinalSequences() {return finalSequences; } 
  vector<SelectedMutation*> getMutations() {return mutationsInSeqs; }
  vector<SelectedMutation*> getFixedMutations(){ return fixedMutations; }

private: 
  vector<BitSet<uint64_t>*> finalSequences;
  vector<SelectedMutation*> mutationsInSeqs;
  vector<SelectedMutation*> fixedMutations;
  
  void annotateFixedMutations(SelectedArray *fixedMutations, BitSet<uint64_t> &fixedNeutral, vector<Node*> &bvMeaning); 
  void annotateRelevantMutations(BitSet<uint64_t> &notPresent, BitSet<uint64_t> &fixedRawMut, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences); 
  void createMergedBitvectors(BitSet<uint64_t> &notPresent, BitSet<uint64_t> &fixedRawMut, vector<BitSet<uint64_t>*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences); 
}; 


bool operator==(SelectedMutation &lhs, SelectedMutation &rhs); 
bool operator==(SelectedMutation &lhs, Node &rhs);   
bool operator==(Node &lhs, SelectedMutation &rhs); 
