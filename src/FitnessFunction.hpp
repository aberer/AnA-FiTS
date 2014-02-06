#pragma once 

#include "common.hpp"
#include "types.hpp"
#include "Randomness.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cassert>
 

enum class FitnessModel { FIXED_S, MIXTURE_GAMMA, NORMAL, FILE };

class FitnessFunction
{
public:   
  FitnessFunction(vector<string> args); 

  float drawSelectionCoefficient(Randomness &rng);
  float getSelectProb(){return 1-probNeutral;}
  seqLen_t getNumberSelected(seqLen_t totalLength); 
  
private:
  FitnessModel mode;
  
  float probPos; // propability for positive effect   
  float probNeutral; // only used to determine the number neutral mutations  
  
  float fixedS; 

  float alpha[2]; 
  float beta[2];

  float mean; 
  float sd; 
};


inline float FitnessFunction::drawSelectionCoefficient(Randomness &rng)
{
  float result = 0; 

  do 
    {
      switch(mode)
	{
	case FitnessModel::FIXED_S:
	  {
	    result = rng.Prob(probPos) ? fixedS : -fixedS; 
	    // std::cout << result << std::endl; 
	  }
	  break; 
	case FitnessModel::MIXTURE_GAMMA:      
	  {
	    result = rng.Prob(probPos) ?  rng.sampleGamma(alpha[0], beta[0]) :  - rng.sampleGamma(alpha[1], beta[1]); 
	    // std::cout <<  result << std::endl; 
	  }
	  break; 
	case FitnessModel::NORMAL: 
	  result = rng.sampleNormal(mean, sd); 
	  break; 
	case FitnessModel::FILE : 
	default: 
	  cerr << "fitness model not implemented yet" << endl; 
	  assert(0); 
	}
    }
  while(result >= 1); 


#ifdef DEBUG_FITNESS_FUNC
  cout << "selCoeff = " << result << endl; 
#endif

  return result;  
}
