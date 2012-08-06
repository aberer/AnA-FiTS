#pragma once 

#include "types.hpp"
#include "common.hpp" 

#include <cassert>
#include <cmath>
#include <RandomLib/Random.hpp>
#include <RandomLib/NormalDistribution.hpp>
#include <RandomLib/ExponentialDistribution.hpp>

using namespace RandomLib; 


class Randomness : public RandomLib::Random
{  
public: 
  Randomness(nat seed); 
  nat samplePoisson(float lambda);  
  float sampleNormal(float mean, float sigma);   
  float sampleGamma(float alpha, float beta); 

  template<typename T, bool neutrality> void sampleValuesByFitness(nat *bufferStart, nat *bufferEnd, FITNESS_TYPE *fitnessValues, nat numberInPool);   
  template<typename TYPE> void IntegerArray(TYPE *array, nat length, nat upperBound); 
  void initArray(uint8_t *array, size_t numBytes); 
  
private:
  NormalDistribution<FITNESS_TYPE> nDist;  
  ExponentialDistribution<float> eDist;
  
  constexpr static float eulerNumber = 2.718281828; 
  constexpr static double eps = 1.2e-07f; 

  // :TRICKY: this could bite you in the back 
  Randomness(const Randomness &rhs);
  Randomness& operator=(const Randomness &rhs);
  float gamma2(float alpha); 
  float gamma1(float alpha); 
  
  template<typename TYPE> inline TYPE getR2(uint32_t tmp); 
}; 


////////////////////
// IMPLEMENTATION //
////////////////////

inline float Randomness::sampleNormal(float mean, float sigma)
{
  return nDist(*this, mean, sigma); 
}


template<typename TYPE> inline TYPE Randomness::getR2(uint32_t tmp)
{
  assert(tmp); 
  TYPE n = tmp-1; 
  n =  n ? n : 1U; 
  // T  n1 = ~number ? number + 1U : 1U;  
  TYPE n1 = n + 1U; 
  assert(n1 > n);  		// asserting that the overflow did not occurr
  TYPE r1 =  (numeric_limits<TYPE>::max() - n) / n1; 
  TYPE theR2 = r1 + 1U; 
  return theR2; 
}

#ifdef HAVE_SSE2
#include "RandomnessImplSSE.hpp"
#else
#include "RandomnessImpl.hpp"
#endif
