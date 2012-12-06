#pragma once 

#include <vector>

#include  "TimeSection.hpp"
#include "common.hpp"
#include "PopulationManager.hpp"
#include "Survivors.hpp"
#include "RegionManager.hpp"
#include "PopulationParams.hpp"
#include "Chromosome.hpp"
#include "ThreadPool.hpp"
#include "RecombinationManager.hpp"
#include "types.hpp"
#include "Graph.hpp"
#include "ChromConfig.hpp"

class Graph; 
class GenerationCounter; 

class Ancestry : private TimeSection
{
public: 
  // LIFE CYCLE 
  Ancestry(ThreadPool &tp, vector<Chromosome*> chromosomes, GenerationCounter &genCnt, const PopulationManager &popMan);
  ~Ancestry();

  // OPERATORS 
  void fillWithRandomIndividuals(ThreadPool &tp); 
  void printGenerations(); 
  template<typename TYPE, bool NEUTRAL> void resampleParentsByFitness(ThreadPool &tp, PopulationManager &popMan, nat generationNum);
  haploAddr getAddrOfParent (nat generation,  nat chromId, haploAddr haploNr ) const;
  void updateGraph(ThreadPool &tp, Chromosome &chromosome, Graph &graph, const PopulationManager &popMan, const GenerationCounter &genCnt);
  void updateGraph_inner(ThreadPool &tp, Survivors &survivors, Chromosome &chromosome, const PopulationManager &popMan, Graph &graph, nat initPopSize); 
  void determineNextSection(GenerationCounter &genCnt, const PopulationManager &popMan);  

  // ACCESSORS
  template<typename TYPE> void initAddrArray (nat generation, nat chromId, nat total, uint32_t *array) const;
  RecombinationManager& getRecombinationManager(nat chromId){return *(recMans[chromId]);}

private:
  // stores the individual's numbers   
  uint8_t *memoryBlockIndi;
  uint8_t **genStartIndi;	// per gen 
  uint32_t *length; 		// per gen 
  nat numChrom;
  vector<ChromConfig<2>*> chromConfig;
  vector<RecombinationManager*> recMans; 
  nat initNum;			// we have to remember, how many individuals we had in the prev gen 

  Ancestry(const Ancestry &rhs); 
  Ancestry& operator=(const Ancestry &rhs);   

  nat initMemBlock(const PopulationManager &popParams); 
  void fillWithRandomIndividuals_parallel(ThreadPool &tp, nat tid); 

  void insertNeutralMutations(ThreadPool& tp,  Graph &graph, const Survivors &survivors, const PopulationManager &popMan, Chromosome &chrom , const RegionManager &regMan, const nat genNum ) ; 

  friend ostream& operator<<(ostream &stream, const Ancestry &rhs);
};


////////////////////
// IMPLEMENTATION //
////////////////////


inline haploAddr Ancestry::getAddrOfParent(nat generation, nat chromId, haploAddr haploNr) const
{
  nat numInPrev = 
    this->getGenIdx(generation) == 0
    ? (DIVIDE_2(initNum) )
    : (DIVIDE_2( length[  this->getGenIdx(generation-1) ]));

  if(numInPrev <= numeric_limits<uint8_t>::max())
    return MULT_2(reinterpret_cast<uint8_t*>(genStartIndi[this->getGenIdx(generation)])[haploNr]) + chromConfig[chromId]->getConfig(generation, haploNr); 
  else if(numInPrev <= numeric_limits<uint16_t>::max())
    return MULT_2( reinterpret_cast<uint16_t*>(genStartIndi[this->getGenIdx(generation)])[haploNr]) + chromConfig[chromId]->getConfig(generation, haploNr); 
  else if(numInPrev <= numeric_limits<uint32_t>::max())
    return MULT_2( reinterpret_cast<uint32_t*>(genStartIndi[this->getGenIdx(generation)])[haploNr]) + chromConfig[chromId]->getConfig(generation, haploNr); 
  else 
    assert(0); 
  return 0; 
}


template<typename TYPE> void Ancestry::initAddrArray(nat generation, nat chromId, nat total, uint32_t *array) const
{
  TYPE *parentArray = reinterpret_cast<TYPE*>(genStartIndi[this->getGenIdx(generation)]); 

  for(nat i = 0; i < total; ++i)    
    array[i] = (MULT_2(parentArray[i]) ) + chromConfig[chromId]->getConfig(generation, i);
}

#define FIX_NUM_ALIGN 80

template<typename TYPE, bool NEUTRAL> void Ancestry::resampleParentsByFitness(ThreadPool &tp, PopulationManager& popMan, nat generationNum)
{
  Randomness &rng = tp[0].getRNG();
  FITNESS_TYPE *fVals  = popMan.getFitnessValues(0); // :KLUDGE: for all populations

  nat numParentsToSample = length[this->getGenIdx(generationNum)];

  nat numIndisPrev = (this->getGenIdx(generationNum) == 0 ? initNum : length[this->getGenIdx(generationNum-1)] ) ; 
  numIndisPrev = DIVIDE_2(numIndisPrev); 

  TYPE *array = reinterpret_cast<TYPE*>(genStartIndi[this->getGenIdx(generationNum)]);

  // cout << "gen=" << generationNum << "\t"
  //      << ( sizeof(TYPE) == 1 ? "uint8_t" : "uint16_t")
  //      << " sampling=" << numParentsToSample << endl; 
  
  assert(numParentsToSample * sizeof(TYPE) <= unsigned(genStartIndi[this->getGenIdx(generationNum) + 1 ] - genStartIndi[this->getGenIdx(generationNum) ])) ; 
  assert(NOT MODULO_16(uint64_t(array) )); 

  TYPE buffer[FIX_NUM_ALIGN] ALIGN(16); 
  constexpr nat numInBuf = FIX_NUM_ALIGN / sizeof(TYPE) ; 
  nat currentNum = 0; 
  rng.IntegerArray(buffer, numInBuf, numIndisPrev); 

  nat i = 0;
  while(i < numParentsToSample)
    {
      if(currentNum == numInBuf -1)
	{
	  rng.IntegerArray(buffer, numInBuf, numIndisPrev); 
	  currentNum = 0; 
	}

      TYPE &elem = array[i];       
      // cout << "checking \t"  << fVals[elem]  << endl; 
      if(elem < numIndisPrev
	 && (NEUTRAL || rng.Prob<FITNESS_TYPE>(fVals[elem])))
	++i; 
      else 	  	  	
	elem = buffer[currentNum++];
    }
}
