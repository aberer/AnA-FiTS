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
  void determineNextSection(const PopulationManager &popMan, nat numChrom);
  bool hasNext();
  void next();

  // ACCESSORS
  nat getStartOfSection() const {return(startOfSection);}
  nat getEndOfSection() const {return(endOfSection);}
  nat getCurrentGeneration() const {return(currentGen);}
  nat getTotalNumGen() const {return(totalNum);}
  bool hasToSimulate() const { return currentGen != totalNum; }
  
private:
  nat currentGen;
  nat totalNum;
  nat startOfSection;
  nat endOfSection;
  long long int memLimit; 
}; 

