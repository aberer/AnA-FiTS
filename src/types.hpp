#pragma once 
#include <climits>
#include <stdint.h>

typedef unsigned int  nat ; 
typedef uint32_t seqLen_t; 
typedef unsigned long long popSize_t; 
typedef unsigned char byte; 


template<typename LARGE, typename PART>
union pun_t
{
  LARGE whole; 
  PART part[sizeof(LARGE) / sizeof(PART)]; 
}; 


#include <functional>
typedef std::function<void(unsigned int)> pFun; 

typedef uint32_t haploAddr; 

#ifdef HAVE_64BIT
typedef uint32_t MEM_TYPE ; 
#else 
typedef uint64_t MEM_TYPE; 
#endif
