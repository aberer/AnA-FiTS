#include "PopulationManager.hpp"


PopulationManager::PopulationManager(vector<PopulationParams> _pp)
  : popParams(_pp)
{
  for(auto popParam : popParams)
    {
      nat numIndi = popParam.getPopSizeByGeneration(0);
      FITNESS_TYPE *someFVals  = (FITNESS_TYPE*)malloc( numIndi  * sizeof(FITNESS_TYPE));
      for(nat i = 0; i < numIndi; ++i)
	someFVals[i] = INIT_FITNESS;   
      fitnessValues.push_back(someFVals); 
    }
}


PopulationManager::~PopulationManager()
{
  for(auto someFVals : fitnessValues)
    free(someFVals); 
}


nat PopulationManager::getMaximumPopSize(nat popId, nat start, nat end) const
{
  nat result = 0; 
  for(nat i = start; i < end; ++i)
    {
      nat tmp = popParams[popId].getPopSizeByGeneration(i); 
      if(tmp > result)
	result = tmp; 
    }

  return result; 
}


#include <iostream>


// :TRICKY: this includes padding
nat PopulationManager::getNumBytesForGeneration(nat gen) const
{
  // KLUDGE once again just one population
  const PopulationParams &popParam = (*this)[0]; 
  nat numHere = popParam.getPopSizeByGeneration(gen) * NUMBER_PARENTS; 
  nat numPrev = (gen == 0 ? popParam.getPopSizeByGeneration(0) : popParam.getPopSizeByGeneration(gen-1)) ; 
  nat bytes = mapToBytesNeeded(numPrev) * numHere; 
  bytes += ADD_PADDING_16(bytes);

  return bytes; 
}

