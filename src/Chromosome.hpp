#pragma once 

#include "common.hpp" 
#include "RecombinationManager.hpp" 
#include "ThreadPool.hpp"
#include "types.hpp"
#include "ResizMemArena.hpp"
#include "BitSet.hpp"
#include "MutationManager.hpp"
#include "SequenceArray.hpp"
#include "DynArraySequential.hpp"
#include "Randomness.hpp"
#include "SeqRep.hpp"
#include "FitnessFunction.hpp"

#include <fstream>

class MutationSelected; 


class Chromosome
{
public:     
  Chromosome(seqLen_t seqLen, bool isNeutral, FitnessFunction fFun, nat id, nat numPop);
  ~Chromosome();
  
  // OPERATORS 
  void cleanFixedMutations(SelectedArray** seqBegin,SelectedArray** seqEnd, nat popId, ThreadPool &tp);   
  SelectedMutation& getSelectedMutation(ResizMemArena<SelectedMutation> &mem, Randomness &rng);
  bool locusIsNeutral(seqLen_t locus);
  void init(Randomness &rng); 

  // ACCESSORS 
  seqLen_t getSeqLen() const { return seqLen; }
  const SeqRep& getSeqRep() const { return seqRep; }
  float getSelectProb(){ assert(mutMan.getSelectProb() <= 1) ; return  isNeutral  ? 0 : mutMan.getSelectProb(); } // 
  float getNeutralProb(){ assert(mutMan.getSelectProb() <= 1) ;  return isNeutral  ? 1 :  1 - mutMan.getSelectProb(); }
  nat getId(){return id; }

  // not really needed by anybody
  SelectedArray* getFixedMutations(nat popId) {return popId >= fixedMutations.size() ? nullptr :  fixedMutations[popId]; } // :KLUDE: this actually belongs to some population-specific object
  Base mutateSite(Randomness &rng, seqLen_t pos){  return mutMan.drawBase(rng, seqRep.getBase(pos)) ; }

private:
  seqLen_t seqLen; 
  DynArraySequential<SelectedArray*> uniqueHaplotypes;
  SeqRep seqRep;
  MutationManager mutMan; 
  nat id;   
  vector<SelectedArray*> fixedMutations; // for each population 
  bool isNeutral;
  
  void insertFixedMutation(SelectedMutation *mut, nat popId); 
  void updateUniqueHaplotypes(SelectedArray** seqBegin,SelectedArray** seqEnd);
  void replaceInAllSequences(nat numMutations, const DynArraySequential<uint16_t> &indicesInOld); 
};


////////////////////
// IMPLEMENTATION //
////////////////////

inline SelectedMutation& Chromosome::getSelectedMutation(ResizMemArena<SelectedMutation>&mem, Randomness &rng)
{
  SelectedMutation &mut = mem.allocate();
  mutMan.initSelectedMutation(rng, mut, seqRep);  
  return mut; 
}


inline bool Chromosome::locusIsNeutral(seqLen_t locus )
{
  return mutMan.locusIsNeutral(locus);
}
