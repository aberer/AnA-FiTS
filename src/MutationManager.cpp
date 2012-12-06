#include "MutationManager.hpp"


MutationManager::MutationManager(seqLen_t _length, FitnessFunction _fFun) 
  : length(_length)
  , numSelected(_fFun.getNumberSelected(length))    
  , isSelected(length)
  , wasEncountered(length)
  , fFun(_fFun)
  , table(numSelected)
  , numNeutral(0)
{
}


Base MutationManager::drawBase(Randomness &rng, const Base &refBase) 
{
  uint8_t theBase = rng.Integer(3);
  if(refBase <= theBase)
    theBase++;
  Base base = Base(theBase); 
  return base; 
}


void MutationManager::initSelectedMutation(Randomness &rng, SelectedMutation &mut, const SeqRep &seqRep)
{
  // :TODO: bottleneck?
  bool selected = false; 
  bool enc = false; 
  bool isSaturated = numSelected == table.getBucketsUsed();

  do 
    {
      mut.absPos = rng.Integer(length); 
      selected = isSelected.test(mut.absPos); 
      enc = wasEncountered.test(mut.absPos); 
    }
  while(NOT selected && (isSaturated || enc) ); 

  if(isSaturated)
    assert(selected); 

  const Base refBase = seqRep.getBase(mut.absPos);
  mut.base = drawBase(rng,refBase); 
  FITNESS_TYPE& fitness =  table[mut.absPos][getPosByBase(mut.base, refBase)]; 
  
  if( ( NOT selected || (selected && fitness == 0.f ) ))
    fitness = 1 + fFun.drawSelectionCoefficient(rng); 

  if(NOT selected)
    {
      wasEncountered.set(mut.absPos); 
      isSelected.set(mut.absPos);
    }

  mut.fitness = fitness; 
}
