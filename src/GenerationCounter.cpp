#include "GenerationCounter.hpp"
#include "BitSet.hpp"

#include <cstdlib>
#include <cassert>

#define MEM_LOAD_FACTOR 0.65


GenerationCounter::GenerationCounter(nat totalNumGen, string _memLimit)
  : currentGen(0)
  , totalNum(totalNumGen)
  , startOfSection(0)
  , endOfSection(0)
{
  if(NOT _memLimit.compare(""))
    memLimit=-1; 
  else 
    {
      nat length = _memLimit.length();
      char lastChar = _memLimit[length-1]; 
      memLimit = atoi(_memLimit.substr(0,length-1).c_str()); 
      switch(lastChar)
	{
	case 'G': case 'g': 
	  memLimit *= 1000; 	// not binary? how evil ... 
	case 'M': case 'm':
	  memLimit *= 1000; 
	case 'K': case 'k': 
	  memLimit *= 1000; 
	  break; 
	default: 		// plain number 
	  memLimit = atoi(_memLimit.c_str());	
	}
    }
}


void GenerationCounter::determineNextSection(const PopulationManager &popMan, nat numChrom)
{
  if(memLimit < 0)
    {
      startOfSection = currentGen; 
      endOfSection = totalNum; 
    }
  else 
    {
      assert(currentGen == endOfSection); 
      startOfSection = currentGen; 
      endOfSection = startOfSection; 

      size_t ancstBytes = 0, 
	numIndi = 0; 
      while(endOfSection < totalNum && 
	    float(ancstBytes + BitSet<uint32_t>::convertToInternalSize((MULT_2(numIndi)) * numChrom))  <  MEM_LOAD_FACTOR * unsigned(memLimit) )
	{
	  ancstBytes += popMan.getNumBytesForGeneration(endOfSection);
	  numIndi += popMan.getTotalNumHaploByGen(endOfSection);
	  endOfSection++; 
	}
    }
}


void GenerationCounter::next()
{
  assert(startOfSection != endOfSection && endOfSection != 0); 
  ++currentGen;
}


bool GenerationCounter::hasNext()
{
  assert(startOfSection != endOfSection && endOfSection != 0); 
  return currentGen < endOfSection; 
}
