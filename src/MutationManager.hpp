#include "Mutation.hpp"
#include "Randomness.hpp"
#include "FitnessFunction.hpp"
#include "SeqRep.hpp"
#include "HashTable.hpp"


class MutationManager
{
public: 
  // LIFE CYCLE 
  MutationManager(seqLen_t length, FitnessFunction fFun); 

  // OPERATORS
  void initSelectedMutation(Randomness &rng, SelectedMutation &mut, const SeqRep &seqRep);
  float getSelectProb(){return fFun.getSelectProb();}
  Base drawBase(Randomness &rng, const Base &refBase); 
  
  // OBSERVERS
  bool locusIsNeutral(seqLen_t locus); 
  
private: 
  seqLen_t length;
  seqLen_t numSelected; 
  BitSet<uint32_t> isSelected;
  BitSet<uint32_t> wasEncountered; 
  FitnessFunction fFun; 
  HashTable table;
  nat numNeutral; 

  uint8_t getPosByBase(const Base &base, const Base &refBase) const; 
}; 



////////////////////
// IMPLEMENTATION //
////////////////////

inline uint8_t MutationManager::getPosByBase(const Base &base, const Base &refBase) const
{
  if(refBase <= base )
    return uint8_t(base) - 1; 
  else 
    return base; 
}



inline bool MutationManager::locusIsNeutral(seqLen_t locus)  
{ 
  bool selP = isSelected.test(locus); 
  bool encP = wasEncountered.test(locus); 

  if(encP)
    return NOT selP; 
  else if(numNeutral < length - numSelected)
    {
      wasEncountered.set(locus); 
      numNeutral++;
      return true; 
    }
  else 
    return false;   
}


