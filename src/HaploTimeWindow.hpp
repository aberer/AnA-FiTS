#pragma once 

#include "common.hpp"
#include "SequenceArray.hpp"
#include "types.hpp"
#include "Ancestry.hpp"


class Chromosome; 

class HaploTimeWindow
{
public:
  // LIFE CYCLE
  HaploTimeWindow(nat initPopSize, SelectedArray *selectedInit) ;
  ~HaploTimeWindow(); 

  // OPERATORS 
  void switchPastAndPresent(); 

  template<typename TYPE> void propagate(const nat generationNum, const Ancestry &ancestry, const nat chromNr, PopulationManager &popMan);

  // ACCESSORS 
  SelectedArray* getPreviousConfiguration(nat num) { return oldState[num]; }
  SelectedArray* getCurrentConfiguration(nat num) { return newState[num]; }
  void setCurrentConfiguration(nat num, SelectedArray *seq) { newState[ num] = seq ; } 
  SelectedArray* at(nat num) {return newState[num]; }
  SelectedArray** start(){return newState; }

private:
  SelectedArray** oldState; 
  SelectedArray** newState;  
  uint32_t *numBuf;  
};




////////////////////
// IMPLEMENTATION //
////////////////////

template<typename TYPE> void HaploTimeWindow::propagate(const nat generationNum, const Ancestry &ancestry, const nat chromNr, PopulationManager &popMan )
{
  nat numberToPropagate = popMan.getTotalNumHaploByGen(generationNum);
  newState = (SelectedArray**)myRealloc(newState, numberToPropagate * sizeof(SelectedArray*)); 
  
  numBuf = (uint32_t*)myRealloc(numBuf, numberToPropagate * sizeof(uint32_t));
  ancestry.initAddrArray<TYPE>(generationNum, chromNr, numberToPropagate, numBuf); 
  
  for(nat i = 0;  i < numberToPropagate; )
    {
      // if(generationNum  > 970  )
      // 	cout << "setting " << i << "/"  << numberToPropagate << ": " << numBuf[i] << endl; 
      newState[i] = oldState[numBuf[i]]; 
      i++;
      // if(generationNum > 970)
      // 	cout << "setting " << i << "/"  << numberToPropagate <<  ": " << numBuf[i] <<  endl; 
      newState[i] = oldState[numBuf[i]]; 
      i++;
    }

  for(nat i = 0; i < numberToPropagate; ++i)
    newState[i]->setGeneration(generationNum); 
}
