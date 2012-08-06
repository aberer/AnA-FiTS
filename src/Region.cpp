#include "Region.hpp"
#include <cstdlib>
#include <algorithm>
#include <cstring>


Region::Region(nat numberArg, seqLen_t seqLenArg, seqLen_t *maxReg)  : 
  number(numberArg),
  seqLen(seqLenArg),
  maxReg(maxReg)
{
  startReg = (seqLen_t*)calloc( numberArg, sizeof(seqLen_t)); 
  endReg = (seqLen_t*)calloc( numberArg, sizeof(seqLen_t)); 
}


Region::~Region()
{
  free(startReg); 
  free(endReg); 
}


void Region::maximizeRegions()
{
  memset(startReg, 0, sizeof(seqLen_t) * number); 
  memcpy(endReg, maxReg, sizeof(seqLen_t) * number); 
}

void Region::minimizeRegions()
{
  memcpy(startReg, maxReg, sizeof(seqLen_t) * number);
  memset(endReg, 0, sizeof(seqLen_t) * number); 
}

