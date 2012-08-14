#include "GenerationCounter.hpp"
#include "BitSet.hpp"
#include "Recombination.hpp"

#include <cstdlib>
#include <cassert>

// #define NUM_IN_POTENCE 1024

void protMult(MEM_TYPE& num, int factor)
{
  MEM_TYPE lastNum = num;
  num *= factor; 

  if(num < lastNum)    
    {
      cerr << "Overflow occurred. Memory parameter is specified larger than integer allows." << endl; 
      abort(); 
    }
}


GenerationCounter::GenerationCounter(nat totalNumGen, string _memLimit)
  : currentGen(0)
  , totalNum(totalNumGen)
  , startOfSection(0)
  , endOfSection(0)
{
  if(NOT _memLimit.compare(""))
    memLimit=0; 
  else 
    {
      nat length = _memLimit.length();
      char lastChar = _memLimit[length-1]; 
      memLimit = atol(_memLimit.substr(0,length-1).c_str()); 
      switch(lastChar)
	{
	case 'G': case 'g': // not binary? how evil ...  
	  protMult(memLimit, memPotency);
	case 'M': case 'm':
	  protMult(memLimit, memPotency); 
	case 'K': case 'k': 
	  protMult(memLimit, memPotency); 
	  break; 
	default: 		// plain number 
	  memLimit = atoi(_memLimit.c_str());	
	}

#ifdef HAVE_64BIT 
#else 
      if(memLimit > numeric_limits<uint32_t>::max())
	cerr << "Build for 32-bit systems. You are possibly requesting more memory than available on your machine." << endl; 
#endif

    }
}

// #define FACTOR 0.9

void GenerationCounter::determineNextSection(const PopulationManager &popMan,  vector<Chromosome*> chromosomes)
{
  if(memLimit < 1000000)
    {
      if(memLimit != 0)
	cerr << "This program won't work well with less than 1MB memory. Ignoring memory constraint." << endl; 

      startOfSection = currentGen; 
      endOfSection = totalNum; 
    }
  else 
    {
      nat numChrom = chromosomes.size();
      assert(currentGen == endOfSection); 
      startOfSection = currentGen; 
      endOfSection = startOfSection; 
      
      MEM_TYPE ancstBytes = 0, 
	recombBytes = 0,
	maxMemForSeqs = 0, 
	total = 0, 
	numIndi = 0; 
      while(endOfSection < totalNum && 
	    total < memLimit )
	{
	  ancstBytes += popMan.getNumBytesForGeneration(endOfSection);

	  MEM_TYPE tmp = 0;
	  for(auto chromosome : chromosomes )
	    { 
	      recombBytes += PopulationManager::getLamdbaForParam(popMan[0].getRecombinationRate(endOfSection), chromosome->getSeqLen(), popMan, 0,endOfSection) * (sizeof(Recombination) + sizeof(Recombination*)); 
	      tmp +=  MEM_TYPE(PopulationManager::getLamdbaForParam(popMan[0].getMutationRate(endOfSection), chromosome->getSeqLen(), popMan, 0,endOfSection) * chromosome->getSelectProb())
		* popMan.getTotalNumHaploByGen(endOfSection) 
		*  (sizeof(SelectedMutation*) + sizeof(SelectedMutation)) ; 
	    }

	  maxMemForSeqs = max(maxMemForSeqs, tmp);
	  numIndi += popMan.getTotalNumHaploByGen(endOfSection);
	  endOfSection++; 
	  
	  total = MEM_TYPE(maxMemForSeqs + ancstBytes + recombBytes + BitSet<uint32_t>::convertToInternalSize((MULT_2(numIndi)))); 
	}

      if(endOfSection < startOfSection + 10)
	{
	  cerr << "This simulation requires more memory." << endl; 
	  abort(); 
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


