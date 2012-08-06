#pragma once 

#include "types.hpp"
#include "common.hpp"

#include <cassert> 

template<class TYPE>
class DynArray
{  
public: 
  explicit DynArray(nat num); 
  ~DynArray(); 
  
  void fullReset(); 
  TYPE& operator[](nat num) {return array[num];}
  
  // OBSERVERS
  nat size() const {return internalSize;}
  TYPE at(nat num) const; 	// :TRICKY: only use with pointers 

protected: 
  TYPE *array;
  nat internalSize;

  void resize();   

private: 
  DynArray<TYPE>& operator=(const DynArray<TYPE> &rhs); 
  DynArray(const DynArray<TYPE> &rhs);
}; 



////////////////////
// IMPLEMENTATION //
////////////////////
template<typename TYPE> DynArray<TYPE>::DynArray(nat num)
  : internalSize(num)
{
  array = (TYPE*)calloc(num, sizeof(TYPE)); 
}  

template<typename TYPE> DynArray<TYPE>::~DynArray()
{
  free(array); 
}



template<typename TYPE> void DynArray<TYPE>::fullReset()
{
  memset(array, 0, internalSize * sizeof(TYPE)); 
}



template<typename TYPE>  TYPE DynArray<TYPE>::at(nat num) const
{
  assert(num < internalSize) ;
  return array[num]; 
}



template<typename TYPE> void DynArray<TYPE>::resize()
{
  internalSize = MULT_2(internalSize);  
  array = (TYPE*) myRealloc(array, internalSize * sizeof(TYPE)); 
}
