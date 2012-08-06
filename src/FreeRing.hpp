#pragma once 

#include "SequenceArray.hpp"


class FreeRing
{
public: 
  FreeRing(nat initNum);
  ~FreeRing();
  SelectedArray* allocate(nat generation);
  
private: 
  SelectedArray **array;
  SelectedArray **lastAllocatedPtr;
  nat capacity;  
  void resize();  
};
