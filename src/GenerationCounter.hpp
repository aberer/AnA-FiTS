#pragma once

#include "types.hpp"
#include "common.hpp"
#include "Ancestry.hpp"

#include <string>

class GenerationCounter
{
public: 
  // LIFE CYCLE
  GenerationCounter(nat totalNumGen, string memLimit); 
  
  // OPERATORS
  void determineNextSection(const PopulationManager &popMan, vector<Chromosome*> chromosomes);
  bool hasNext();
  void next();

  // ACCESSORS
  nat getStartOfSection() const {return(startOfSection);}

  // the id is exclusive!!!
  nat getEndOfSection() const {return(endOfSection);}
  // :TRICKY: use that when you want the last id (instead of the previous function )
  nat getIdOfLastGenInSection() const {return endOfSection-1; }

  nat getCurrentGeneration() const {return(currentGen);}
  nat getTotalNumGen() const {return(totalNum);}
  bool hasToSimulate() const { return currentGen != totalNum; }

private:
  constexpr static int memPotency = 1024;  
  
  nat currentGen;
  nat totalNum;
  nat startOfSection;
  nat endOfSection;
  MEM_TYPE memLimit; 
}; 

