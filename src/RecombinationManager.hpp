#pragma once 

#include "common.hpp"
#include "PopulationManager.hpp"
#include "PopulationParams.hpp"
#include "TimeSection.hpp"
#include "Randomness.hpp"
#include "ThreadPool.hpp"


int sortByAbsPos(const void* t1, const void* t2);

class SimulationPart; 


class RecombinationManager : private TimeSection
{
public: 
  explicit RecombinationManager(seqLen_t seqLen, nat startGen, nat endGen); 
  ~RecombinationManager();

  void precompute(ThreadPool &tp, const PopulationManager &popParams); 
  nat getNumRecByGen(nat gen) const {return numRecPerGen.at(this->getGenIdx(gen));}
  Recombination* getFirstRecByGen(nat gen){return firstRecPerGen.at(this->getGenIdx(gen)); }
  Recombination* getLastRecByGen(nat gen){return firstRecPerGen.at(this->getGenIdx(gen)) +  numRecPerGen.at(this->getGenIdx(gen)); }
  void obtainRecsForIndividual(Recombination *&start, nat &length); 
  
private: 
  nat totalRec;
  Recombination *recombs;  
  nat recIndex;
  vector<Recombination*> firstRecPerGen; 
  vector<nat> numRecPerGen;
  seqLen_t seqLen; 
  
  void determineRecsPerGen(ThreadPool &tp , const PopulationManager &popMan);   
  template<typename T> void precompute_helper(ThreadPool &tp, uint32_t*& numRecPerIndi, nat numRec, nat maxIndi); 

  friend ostream& operator<<(ostream &stream, const RecombinationManager &rhs);     
}; 


////////////////////
// IMPLEMENTATION //
////////////////////
template<>
inline void RecombinationManager::precompute_helper<uint8_t>(ThreadPool &tp, uint32_t*& numRecPerIndi, nat numRec, nat maxIndi)
{
  pun_t<uint32_t, uint8_t> converter;   
  nat numSampled = 0; 
  nat m = 0; 
  nat r2 = 0; 
  Randomness &rng = tp[0].getRNG();
  
  if(numRec > 4 )
    {
      intNormalizer<uint8_t>(maxIndi, m,r2); 
      numRec -= 4;
      while(numSampled < numRec)
	{
	  converter.whole = rng.Ran32();
	  if(converter.part[0] <= m) { nat nr = converter.part[0] / r2; ++numRecPerIndi[nr]; ++numSampled; }
	  if(converter.part[1] <= m) { nat nr = converter.part[1] / r2; ++numRecPerIndi[nr]; ++numSampled; }
	  if(converter.part[2] <= m) { nat nr = converter.part[2] / r2; ++numRecPerIndi[nr]; ++numSampled; }
	  if(converter.part[3] <= m) { nat nr = converter.part[3] / r2; ++numRecPerIndi[nr]; ++numSampled; }
	}
      
      numRec += 4; 
    }

  while(numSampled < numRec)
    {
      ++numRecPerIndi[rng.Integer<nat>(maxIndi)]; 
      ++numSampled; 
    }  
}



template<>
inline void RecombinationManager::precompute_helper<uint16_t>(ThreadPool &tp, uint32_t*& numRecPerIndi, nat numRec, nat maxIndi)
{
  pun_t<uint32_t, uint16_t> converter;   
  nat numSampled = 0; 
  nat m = 0; 
  nat r2 = 0; 
  Randomness &rng = tp[0].getRNG();
  
  if(numRec > 2 )
    {
      intNormalizer<uint16_t>(maxIndi, m,r2); 
      numRec -= 2;
      while(numSampled < numRec)
	{
	  converter.whole = rng.Ran32();
	  if(converter.part[0] <= m) { nat nr = converter.part[0] / r2; numRecPerIndi[nr]++; numSampled++; }
	  if(converter.part[1] <= m) { nat nr = converter.part[1] / r2; numRecPerIndi[nr]++; numSampled++; }
	}
      
      numRec += 2; 
    }

  while(numSampled < numRec)
    {
      ++numRecPerIndi[rng.Integer<nat>(maxIndi)]; 
      ++numSampled; 
    }
}


template<>
inline void RecombinationManager::precompute_helper<uint32_t>(ThreadPool &tp, uint32_t*& numRecPerIndi, nat numRec, nat maxIndi)
{
  Randomness &rng = tp[0].getRNG();
  nat numSampled = 0; 

  while(numSampled < numRec )
    {
      ++numRecPerIndi[rng.Integer<nat>(maxIndi)]; 
      ++numSampled; 
    }    
}

inline void RecombinationManager::obtainRecsForIndividual(Recombination *&start, nat &length)
{
  if(NOT (recIndex <  totalRec))
    return; 

  start = &(recombs[recIndex]);
  length++; 
  recIndex++; 
    
  while(recIndex < totalRec && start->haploIndiNr == recombs[recIndex].haploIndiNr)
    {
      recIndex++; 
      length++;
    }
  
  assert(recIndex <= totalRec); 
    
  if(length > 1)
    qsort(start, length, sizeof(Recombination), sortByAbsPos);
}

