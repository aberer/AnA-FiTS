#include "FreeRing.hpp"
#include <iostream>


FreeRing::FreeRing(nat initNum)
  : capacity(initNum)
{
  array = (SelectedArray**) malloc(capacity * sizeof(SelectedArray*)); 
  for(nat i = 0; i < capacity ; ++i)
    array[i] = new SelectedArray(1000);	// :MAGIC:
  lastAllocatedPtr = array;   
}

FreeRing::~FreeRing()
{
  for(nat i = 0; i < capacity; ++i)
    delete array[i]; 
  free(array); 
}

void FreeRing::resize()
{
  nat newCapacity = 2 * capacity; 
  array = (SelectedArray**)myRealloc(array, newCapacity * sizeof(SelectedArray*)); 
  for(auto i = array + capacity ; i < array + newCapacity ; ++i)
    *i = new SelectedArray(10);	// :MAGIC:
  lastAllocatedPtr = array + capacity; 
  capacity = newCapacity;   
}


SelectedArray *FreeRing::allocate(nat inGeneration)
{  
  SelectedArray *result = nullptr;

  auto end  = array + capacity; 
  for(auto iter = lastAllocatedPtr; iter < end; ++iter)
    {
      nat gen = (*iter)->getGeneration();
      if(inGeneration - gen > 1)
	{
	  result = *iter;
	  lastAllocatedPtr = iter;
	  result->setGeneration(inGeneration); 
	  result->setCreationGen(inGeneration); 
	  result->setFitness(1.0) ; 
	  return result; 
	}
    }
  
  for(auto iter = array ; iter < lastAllocatedPtr; ++iter)
    {
      nat gen = (*iter)->getGeneration(); 
      if(inGeneration - gen > 1)
	{
	  result = *iter; 
	  lastAllocatedPtr = iter; 
	  result->setGeneration( inGeneration); 
	  result->setCreationGen(inGeneration); 
	  result->setFitness(1.0) ; 
	  return result; 
	}
    }
  
  lastAllocatedPtr = array + capacity;
  resize();
  lastAllocatedPtr++; 
  result = *lastAllocatedPtr;  
  assert(inGeneration == 0 || (result->getGeneration() != inGeneration && result->getGeneration() != inGeneration - 1 ));
  result->setGeneration(inGeneration); 
  result->setCreationGen(inGeneration); 
  result->setFitness(1.0) ; 
  return result; 
}
