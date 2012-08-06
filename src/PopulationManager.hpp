#pragma once 

#include "PopulationParams.hpp"
#include "common.hpp"


class PopulationManager
{
public: 
  PopulationManager(vector<PopulationParams> popParams);
  ~PopulationManager();

  // ACCESSORS
  nat getTotalNumHaploByGen(nat num) const; 
  nat getTotalNumIndiByGen(nat num) const;
  nat getTotalNumPopulations() const {return popParams.size(); } // :TRICKY: at all times!   
  nat getMaximumPopSize(nat popId, nat start, nat end) const ; 
  FITNESS_TYPE* getFitnessValues(nat popId){return fitnessValues[popId];}
  void setFitnessValues(nat popId, FITNESS_TYPE *fval){fitnessValues[popId] = fval;}  
  const PopulationParams& operator[](nat num) const { return popParams[num]; }  
  nat getNumBytesForGeneration(nat gen) const; 

  static double getLamdbaForParam(float baseParam, seqLen_t seqLen, const PopulationManager &popMan, nat popNum, nat gen) ; 
  static uint8_t mapToBytesNeeded (nat number); 
  
private:   
  vector<PopulationParams> popParams;
  vector<FITNESS_TYPE*> fitnessValues;
};


////////////////////
// IMPLEMENTATION //
////////////////////

inline nat PopulationManager::getTotalNumHaploByGen(nat num) const
{
  return MULT_2(getTotalNumIndiByGen(num)); 
}

inline nat PopulationManager::getTotalNumIndiByGen(nat num) const
{
  nat result = 0; 
  for(auto &popP : popParams)
    result += popP.getPopSizeByGeneration(num);
  return result; 
}




inline double PopulationManager::getLamdbaForParam(float baseParam, seqLen_t seqLen, const PopulationManager &popMan, nat popNum, nat gen) 
{ 
  return MULT_2(popMan[popNum].getPopSizeByGeneration(gen)) * baseParam * seqLen ; // 2 N_e param seqLen
}


inline uint8_t PopulationManager::mapToBytesNeeded(nat number) 
{
  uint8_t result = 0; 

  if(number < numeric_limits<uint8_t>::max())
    result = 1 ; 
  else if(number < numeric_limits<uint16_t>::max())
    result = 2 ; 
  else if(number < numeric_limits<uint32_t>::max())
    result = 4 ; 
  else 
    assert(0);
  
  return result; 
}
