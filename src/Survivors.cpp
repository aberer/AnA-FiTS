#include "Survivors.hpp"


Survivors::Survivors(nat lastPopsize, nat maxPopSize, nat _startGen, nat _endGen)
  : TimeSection(_startGen, _endGen)
  , capacity(maxPopSize * 10 )
  , setMaker(MULT_2(maxPopSize))
  , highestIndex(this->getEndGen() - this->getStartGen() -1)
  , currentGenerationBackwards(0) 
{
  survivors = (int*)calloc(capacity, sizeof(int));
  nextSurvivor = &(this->survivors[0]);

  survivorAccCountByGen = (nat*)malloc( sizeof(nat) * (this->getEndGen() - this->getStartGen() +2)); 
  survivorAccCountByGen[0] = 0;  

  initializePresentGeneration(lastPopsize);
}


Survivors::~Survivors()
{
  free(survivorAccCountByGen); 
  free(survivors); 
}


void Survivors::resize()
{
  nat offset = nextSurvivor - survivors; 

  capacity = MULT_2(capacity);
  survivors = (int*)myRealloc(survivors, capacity * sizeof(int));

  nextSurvivor = survivors + offset;   
}


void Survivors::initializePresentGeneration(nat howMany)
{
  reserve(howMany); 

  for(nat i = 0; i < howMany; ++i)
    {
      *nextSurvivor = i; 
      ++nextSurvivor;
    }  
  
  prepareNextGenBackwards(); 	
}
