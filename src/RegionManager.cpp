#include "RegionManager.hpp"

#include <algorithm>
#include <iostream>

RegionManager::RegionManager(nat maxPopSize, seqLen_t  seqLen)
  {
    maxReg = (seqLen_t*)calloc(maxPopSize, sizeof(seqLen_t));
    for(nat i = 0; i < maxPopSize; ++i) 
      maxReg[i] = seqLen;

    regionNow = new Region(maxPopSize, seqLen, maxReg); 
    regionPrev =  new Region(maxPopSize, seqLen, maxReg); 
    
    // everything survived into current generation
    regionNow->maximizeRegions();
    regionPrev->minimizeRegions();
  }


RegionManager::~RegionManager()
  {
    delete regionNow; 
    delete regionPrev; 
    free(maxReg); 
  }  

void RegionManager::nextGenBackwards() 
  {
    std::swap(regionNow, regionPrev) ; 
    regionPrev->minimizeRegions();
  }
