#include "HashTable.hpp"

#include <cassert>
#include <iostream>


seqLen_t findNextPower2(seqLen_t num)
{
  seqLen_t v = num; 
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v; 
}


HashTable::HashTable(nat numSites) 
  : bucketsUsed(0)
{  
  buckets = (FitnessBucket*)calloc(numSites, sizeof(FitnessBucket)); 
  nat nextPower = findNextPower2(numSites); 
  array = (FitnessBucket**)calloc(nextPower, sizeof(FitnessBucket*));
  moduloFactor = nextPower - 1; 
}


HashTable::~HashTable()
{
  free(array); 
  free(buckets); 
}

