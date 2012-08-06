#pragma once 
#include "common.hpp"
#include "DynArray.hpp"


template <class TYPE>
class DynArraySequential : public DynArray<TYPE>
{ 
public:
  DynArraySequential(nat num);

  void resetUsed(); 
  nat getUsed () const ; 
  void setNext(TYPE type); 
  TYPE& getNext();
  TYPE at(nat num) const; 
  TYPE* begin() const; 
  TYPE* end() const; 
  TYPE& pop(); 

private: 
  nat used; 
  DynArraySequential<TYPE>& operator=(const DynArraySequential<TYPE> &rhs); 
  DynArraySequential(const DynArraySequential<TYPE> &rhs); 

};


////////////////////
// IMPLEMENTATION //
////////////////////


template<class T>
DynArraySequential<T>::DynArraySequential(nat num)
  : DynArray<T>(num) 
  , used(0)
{
}

template<class T>
void DynArraySequential<T>::resetUsed()
{
  memset( this->array, 
	  0, 
	  used * sizeof(T)); 
  used = 0; 
}


template<class T>
nat DynArraySequential<T>::getUsed () const 
{
  return used; 
} 

template<class T>
void DynArraySequential<T>::setNext(T type)
{
  if(used == this->internalSize-1)
    this->resize();  
  this->array[(this->used)++] = type;
}


template<class T>
T DynArraySequential<T>::at(nat num) const
{
  assert(num < used) ; 
  return ((DynArray<T>*)(this))->at(num); 
}


template<class T>
T* DynArraySequential<T>::begin()  const
{
  return this->array; 
}

template<class T>
T* DynArraySequential<T>::end() const
{
  return this->array + used; 
}


template<class T>
T& DynArraySequential<T>::getNext()
{
  if(used == this->internalSize-1)
    this->resize();

  return this->array[used++];   
}


template<class T>
T& DynArraySequential<T>::pop()
{
  assert(used);   
  T& result = this->array[used-1]; 
  --used;
  return result; 
}
