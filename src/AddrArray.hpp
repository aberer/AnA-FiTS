#pragma once 

/** 
    AddrArray is a dynamically growing array. In contrast to DynArray,
    the addresses to its members are preserved however. This means,
    that the pointers to elements of the array can be set elsewhere
    and the resize operation will not invalidate those.    
    
    It is intended to be used with another data structure, this class
    alone just provides addresses to the next elements from a memory
    management perspective.
 */ 

#include "common.hpp"
#include "types.hpp"

#include <iostream>


//////////////
// ITERATOR //
//////////////

template<class VALUE,bool ALLOC_CLEAN>
class AddrArrayBackwardIter
{
 public:
  bool back();
  VALUE* operator()() { return &(arrays[arrayNum-1][index]) ;}
  bool empty(){return index == 0 && arrayNum == 1; }
  
 // private: 
  VALUE **arrays; 
  nat arrayNum; 
  nat index;
  nat currentCapacity;   
}; 



template<class VALUE,bool ALLOC_CLEAN>
inline bool AddrArrayBackwardIter<VALUE,ALLOC_CLEAN>::back()
{
  if(index == 0)
    {
      if(arrayNum == 1)
	return false; 
      else 
	{
	  arrayNum--; 
	  currentCapacity = currentCapacity >> 1; 
	  index = currentCapacity-1; 
	  return true; 
	}
    }
  else 
    {
      index--;
      return true; 
    }
}


template<class VALUE, bool ALLOC_CLEAN>
class AddrArray
{
public: 
  explicit AddrArray(nat initSize); 
  ~AddrArray();
  
  VALUE* getNext(nat num);
  VALUE* getNext();
  nat getNumberUsed(){return totalUsed;}
  void getEnd(AddrArrayBackwardIter<VALUE,ALLOC_CLEAN> &result); 

private: 
  VALUE **arrays;
  nat numArrays; 
  nat currentCapacity; 
  nat used; 
  nat totalUsed; 

  void resize();
  AddrArray(const AddrArray &rhs); 
  AddrArray& operator=(const AddrArray &rhs); 
  
  friend class AddrArrayBackwardIter<VALUE,ALLOC_CLEAN>; 
};


////////////////////
// IMPLEMENTATION //
////////////////////
 
template<class VALUE, bool ALLOC_CLEAN>AddrArray<VALUE,ALLOC_CLEAN>::AddrArray(nat initSize)
  : numArrays(1)
  , currentCapacity(initSize)
  , used(0)  
  , totalUsed(0)
{
  arrays = (VALUE**)malloc(sizeof(VALUE*));  

  if(ALLOC_CLEAN)
    arrays[0] = (VALUE*)calloc(currentCapacity, sizeof(VALUE));
  else 
    arrays[0] = (VALUE*)malloc(currentCapacity * sizeof(VALUE)); 
}


template<class VALUE, bool ALLOC_CLEAN>
inline void AddrArray<VALUE,ALLOC_CLEAN>::getEnd(AddrArrayBackwardIter<VALUE,ALLOC_CLEAN> &result)
{
  result.arrays = arrays; 
  result.index = used; 
  result.arrayNum =  numArrays; 
  result.currentCapacity  = currentCapacity; 

  result.back();
}


template<class VALUE, bool ALLOC_CLEAN>AddrArray<VALUE, ALLOC_CLEAN>::~AddrArray()
{
  for(nat i = 0; i < numArrays; ++i)
    free(arrays[i]); 
  free(arrays); 
}


template<class VALUE, bool ALLOC_CLEAN>void AddrArray<VALUE, ALLOC_CLEAN>::resize()
{
  numArrays++;
  arrays = (VALUE**)myRealloc(arrays, numArrays * sizeof(VALUE*));

  used = 0;   
  currentCapacity *= 2 ; 

  if(ALLOC_CLEAN)
    arrays[numArrays-1] = (VALUE*)calloc(currentCapacity, sizeof(VALUE)); 
  else
    arrays[numArrays-1] = (VALUE*)malloc(currentCapacity * sizeof(VALUE));     
}


template<class VALUE, bool ALLOC_CLEAN> VALUE* AddrArray<VALUE, ALLOC_CLEAN>::getNext(nat num)
{
  if(used + 3 >= currentCapacity)
    resize();

  VALUE *result = arrays[numArrays-1] + used; 
  used += num; 
  totalUsed += num; 
  return result;
}


template<class VALUE, bool ALLOC_CLEAN> VALUE* AddrArray<VALUE,ALLOC_CLEAN>::getNext()
{
  if(used == currentCapacity)
    resize();
  
  VALUE *result = arrays[numArrays-1] + used; 
  ++used; 
  ++totalUsed;
  return result; 
}

