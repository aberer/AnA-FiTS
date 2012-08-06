#include "common.hpp"
#include "Region.hpp"

class RegionManager
{  
public: 
  RegionManager(nat maxPopSize, seqLen_t  seqLen);
  ~RegionManager(); 
  
  void nextGenBackwards(); 
  
  seqLen_t getRegionStart(nat idx ) {  return regionNow->startReg[idx]; }
  seqLen_t getRegionEnd(nat idx ) { return regionNow->endReg[idx]; } 

  void extendPrevRegion(nat idx, seqLen_t start, seqLen_t stop ) { regionPrev->extendBy(idx, start, stop); }
  bool locusSurvivesInPresent(nat haploNr, seqLen_t locus )  const { return (locus  >= regionNow->startReg[haploNr] ) && (regionNow->endReg[haploNr] >= locus )  ; } 

private: 
  Region *regionPrev; 
  Region *regionNow;   
  seqLen_t *maxReg;

  friend class RegionoManagerTest; 
}; 


