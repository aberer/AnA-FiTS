#pragma once  

#include "common.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>


template<class VALUE>
class ResizMemArena
{
public: 
  explicit ResizMemArena(nat initSize); 
  ~ResizMemArena(); 
  VALUE& allocate();
  nat getSlotsToCheck() const;
  void unclaimAll(); 
  
private: 
  VALUE **arrays;
  nat numArrays; 
  nat initSize;
  
  nat currentArraySize; 
  nat currentArray; 
  nat currentIndex; 

  void resize(); 
  ResizMemArena& operator=(const ResizMemArena &rhs) = delete; 
  ResizMemArena(const ResizMemArena &rhs) = delete;
};



////////////////////
// IMPLEMENTATION //
////////////////////
template<class VALUE> ResizMemArena<VALUE>::ResizMemArena(nat _initSize)  
  : numArrays(1)
  , initSize(_initSize)
  , currentArraySize(initSize)
  , currentArray(0)
  , currentIndex(0)
{
  arrays = (VALUE**)malloc(sizeof(VALUE*)); 
  arrays[0] = (VALUE*)calloc(initSize, sizeof(VALUE)); 
}


template<class VALUE> ResizMemArena<VALUE>::~ResizMemArena()
{
  for(nat i = 0; i < numArrays; ++i)
    free(arrays[i]); 
  free(arrays); 
}


template<class VALUE> VALUE& ResizMemArena<VALUE>::allocate()
{ 
  bool found = false; 
  VALUE *result = nullptr; 
  
  while( NOT found )
    {
      bool inLastArray = currentArray == numArrays - 1; 
      bool atLastIndex = currentIndex == currentArraySize -1; 

      if(inLastArray && atLastIndex)
	resize();
      else if(atLastIndex)
	{	  
	  currentArray++; 
	  currentIndex = 0;
	  currentArraySize = initSize * POWER_2(currentArray-1);
	}      

      result = &(arrays[currentArray][currentIndex]); 
      currentIndex++; 
      found = NOT result->isClaimed();
    }
  
  result->claim();  
  return *result; 
}


template<class VALUE> void ResizMemArena<VALUE>::resize()
{
  ++numArrays; 
  arrays = (VALUE**)myRealloc(arrays, numArrays * sizeof(VALUE*)); 
  nat nextSize = initSize * POWER_2(numArrays-1); 
  arrays[numArrays-1] = (VALUE*)calloc(nextSize, sizeof(VALUE)); 

  currentArraySize = nextSize; 
  currentArray++; 
  currentIndex = 0; 

  // cout << "RFR resize: allocating new array with size " << nextSize << endl;
}


template<class VALUE> nat ResizMemArena<VALUE>::getSlotsToCheck() const
{
  nat result = initSize * POWER_2(currentArray) - currentIndex;

  for(nat i = currentArray+1; i < numArrays; ++i)    
    result +=initSize * POWER_2(i-1); 

  return result; 
}



template<class VALUE> void ResizMemArena<VALUE>::unclaimAll()
{
  for(nat i = 0; i < numArrays; ++i)
    {
      nat size = initSize * POWER_2(i); 
      for(nat j = 0; j < size; ++j)
	arrays[i][j].unclaim();
    }
  
  currentArray = 0; 
  currentIndex = 0; 
  currentArraySize = initSize; 
}
