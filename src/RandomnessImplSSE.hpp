#include "SseWrapper.hpp"

#include <cassert>


inline __m128i tamper(__m128i part)
{
  static const __m128i numA = _mm_set1_epi32(0x9d2c5680UL); 
  static const __m128i numB = _mm_set1_epi32(0xefc60000UL);

  part = XOR(part, SHIFT_RIGHT_32(part,11));
  part = XOR(part, AND(SHIFT_LEFT_32(part, 7),numA));
  part = XOR(part, AND(SHIFT_LEFT_32(part,15),numB));
  part = XOR(part, SHIFT_RIGHT_32(part,18));
  return  part;
}

template<> void Randomness::IntegerArray(uint8_t *array, nat length, nat upperBound); 

template<> inline void Randomness::IntegerArray(uint8_t *array, nat length, nat upperBound)
{
  __m128i *startEfficient  = reinterpret_cast<__m128i*>(array);   
  nat r2 = getR2<uint8_t>(upperBound);
  size_t numBytes = length; 
  numBytes += ADD_PADDING_16(numBytes); 
  initArray(reinterpret_cast<uint8_t*>(startEfficient), numBytes); 
  nat iterations = numBytes / sizeof(__m128i); 
  for(nat i = 0; i < iterations; ++i)
    {
      __m128i part = startEfficient[i]; 
      tamper(part); 
      startEfficient[i] = part;       
    }
  
  for(nat i = 0; i < numBytes; ++i)
    array[i] /= r2; 
}

template<typename TYPE> void Randomness::IntegerArray(TYPE *array, nat length, nat upperBound)
{ 
  assert(NOT MODULO_16((uint64_t)array)); 

  assert(sizeof(TYPE) > 1); 

  static const nat byteInType = sizeof(TYPE); 
  __m128i *startEfficient  = reinterpret_cast<__m128i*>(array);   

  __m128i r2_par[2]; 
  setDivisor<TYPE>(r2_par, getR2<TYPE>(upperBound)); 

  size_t numBytes = byteInType * length;
  numBytes += ADD_PADDING_16(numBytes); 

  // write random bits into array  
  initArray(reinterpret_cast<uint8_t*>(startEfficient), numBytes);
  nat iterations = numBytes / sizeof(__m128i) ;  
  for(uint32_t i = 0; i < iterations; ++i)
    {      
      __m128i part = startEfficient[i]; 
      
      // tampering the 32-bits
      tamper(part);
      
      // divide 
      part = divide<TYPE>(r2_par, part);
      startEfficient[i] = part;      
    }
}



