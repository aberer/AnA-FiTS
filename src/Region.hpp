#include "common.hpp"
#include "types.hpp"


class Region
{
public :  
  seqLen_t *startReg; 
  seqLen_t *endReg;
  
  Region(nat number, seqLen_t seqLen, seqLen_t *maxReg );
  ~Region(); 

  void maximizeRegions();
  void minimizeRegions();
  void extendBy(nat indiNr, seqLen_t start, seqLen_t end); 

private: 
  nat number; 
  seqLen_t seqLen; 
  seqLen_t *maxReg; 
  
  friend class RegionTest;
}; 


inline void Region::extendBy(nat indiNr, seqLen_t start, seqLen_t end)
{
  startReg[indiNr] = min(startReg[indiNr], start); 
  endReg[indiNr] = max(endReg[indiNr],end);  
}
