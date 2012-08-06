#include "Randomness.hpp"
#include "types.hpp"

#include <cstring>
#include <cassert>
#include <cmath>


Randomness::Randomness(nat seed)
  : RandomLib::Random(seed)
{
}


void Randomness::initArray(uint8_t *array, size_t numBytes)
{
  size_t haveBytes = 0; 
  while(numBytes > 0)
    {
      if(_ptr >= N)
	Next();
      else
	{
	  size_t bytesLeft = (this->N - this->_ptr) * sizeof(uint32_t) ; // :MAGIC: 32 bit
	  size_t toCopy = bytesLeft > numBytes ? numBytes : bytesLeft;	  
	  memcpy(array + haveBytes, _state + _ptr, toCopy);
	  _ptr += DIVIDE_4(toCopy);
	  numBytes -= toCopy;
	  haveBytes += toCopy; 
	}
    }
}


// :TODO: improve
nat Randomness::samplePoisson(float lambda)
{
  if( lambda > 10 )		// :MAGIC:
    {
      RandomLib::NormalDistribution<float> nDist; 
      int num = round(nDist(*this, lambda, sqrt(lambda))); 
      return (num < 0) ? 0 : num;
    }
  else
    {
      int count = 0; 
      float bound = exp(-lambda);   
      for(float q = 1; q >= bound ; q *= Fixed<float>())
	count++; 
      return count -1 ; 
    }
}


float Randomness::gamma2(float alpha)
{
  double uniform1,uniform2;
  double c1,c2,c3,c4,c5,w;
  double random;
  int done = 1;
  c1 = alpha - 1;
  c2 = (alpha - 1/(6 * alpha))/c1;
  c3 = 2 / c1;
  c4 = c3 + 2;
  c5 = 1 / sqrt(alpha);
  do
    {
      uniform1 = Fixed(); 
      uniform2 = Fixed();
      if (alpha > 2.5)
	uniform1 = uniform2 + c5 * (1 - 1.86 * uniform1);
    }
  while ((uniform1 >= 1) || (uniform1 <= 0));
  w = c2 * uniform2 / uniform1;
  if ((c3 * uniform1 + w + 1/w) > c4)
    {
      float foo=c3 * log(uniform1);
      foo+=w;
      foo-=log(w);
      if (foo  >= 1)	
	done = 0;
    }
  if (done == 0)
    return -1;
  random = c1 * w;
  return random;
}


float Randomness::gamma1(float alpha)
{
  double uniform0, uniform1;
  float result, x;
  uniform0 = Fixed();
  uniform1 = Fixed();
  if (uniform0 > eulerNumber/(alpha + eulerNumber))
    {
      result = -log((alpha + eulerNumber)*(1-uniform0)/(alpha*eulerNumber));
      if ( uniform1 > pow(result,alpha - 1))
	return -1;
      else 
	return result;
    }
  else
    {
      x = (alpha + eulerNumber) * uniform0 / eulerNumber;
      result = pow(x,1/alpha);
      if ( uniform1 > exp(-result))
	return -1;
      else
	return result;
    } 
}


float Randomness::sampleGamma(float alpha, float beta)
{
  float result = 0.0;
  double invertBeta = 1 / beta; 

  if (alpha < 1)
    do 
      result = gamma1(alpha) * invertBeta; 
    while (result < 0 );
  
  if (fabs(alpha - 1)< eps)
    result = eDist(*this) * invertBeta; 

  if (alpha > 1)
    do 
      result = gamma2(alpha) * invertBeta; 
    while (result < 0);

  return result;
}

