#pragma once 

#include <cassert>

#include "Judy.h"
#include "common.hpp"

typedef unsigned long judyKey;

template <class Index, class Value> 
class JudyArray {
  void* jarray;

public: 
  JudyArray(); 
  ~JudyArray();   
  
  void set(Index index, Value value);   
  Value* get(Index index);   
  bool first(Index &index, Value &result);   
  bool next(Index index, Value &result);  
  bool searchFirstEqualOrGreater(Index index, Value &result) const; 
  bool getNextHighest(Index index, Value &result); 
  bool searchLastEqualOrLess(Index index, Value &result) const; 
  
  
  // TODO that would be nice  
  Value& operator[](Index index)
  {
    void *val; 
    JLG(val, jarray, index); 
    if(NOT val)
      JLI(val, jarray, index); 
    return *((Value*)val); 
  }
}; 


////////////////////
// IMPLEMENTATION //
////////////////////

template <class Index, class Value> JudyArray<Index,Value>::JudyArray()
{
  if(sizeof(Value) > sizeof(void*))
    assert(0); 
  jarray = 0; 
}


template <class Index, class Value>  JudyArray<Index,Value>::~JudyArray()
{
  if( NOT jarray)
    return; 
  int rc ;  
  // :BUG: tiny memory bug, unique_ptr should be used 
  // JLFA(rc,jarray); 
}



template <class Index, class Value> void JudyArray<Index,Value>::set(Index index, Value value)
{
  void *val; 
  JLI(val, jarray, index); 
  *((Value*)val) = value;  
}


template <class Index, class Value>   Value* JudyArray<Index,Value>::get(Index index)
{
  void *val ; 
  JLG(val, jarray, index);
  return (Value*)val; 
}


template <class Index, class Value>   bool JudyArray<Index,Value>::first(Index &index, Value &result)
{
  void *val; 
  JLF(val, jarray, index); 

  if(val)
    result = *((Value*)val); 

  return val != nullptr; 
}
  

template <class Index, class Value> bool JudyArray<Index,Value>::next(Index index, Value &result)
{
  void *val; 
  JLN(val,jarray, index); 

  if(val)
    result = *((Value*)val);     
    
  return val != nullptr; 
} 
  

template <class Index, class Value>   bool JudyArray<Index,Value>::searchFirstEqualOrGreater(Index index, Value &result) const
{
  void *val;
  JLF(val, jarray, index); 
    
  if(val)
    result = *((Value*)val); 
    
  return val != nullptr; 
}


template <class Index, class Value>   bool JudyArray<Index,Value>::getNextHighest(Index index, Value &result)
{
  void *val;
  JLF(val, jarray, index); 
    
  if(val)
    result = *((Value*)val); 
    
  return val != nullptr; 
}


template <class Index, class Value>   bool JudyArray<Index,Value>::searchLastEqualOrLess(Index index, Value &result) const
{
  void *val; 

  Index tmpIndex = 0;  
  JLF(val,jarray,tmpIndex); 
  if( NOT val )
    return false; 

  JLL(val, jarray, index) ; 
  if(val)
    result = *((Value*)val);

  return val != nullptr;
}
