#pragma once 

#include "types.hpp"
#include "Randomness.hpp"
#include "TimeSection.hpp"

template<nat PLOIDY>
class ChromConfig : private TimeSection
{
public:
  ChromConfig(nat numIndi, uint32_t *lengths, nat startGen, nat endGen); 
  ~ChromConfig();  
  nat getConfig(nat gen, haploAddr haploNr); 
  
  template<typename TYPE> void getBothConfig(nat gen, haploAddr firstNr, TYPE* result1, TYPE* result2)
  {  
    nat pos = startByGen[gen] + firstNr;
    nat rest = MODULO_8(pos);
  
    if(rest == 7)
      {
	nat divided = DIVIDE_8(pos); 
	*result1 = (*result1 << 1) +  ((byteRepresentation[divided] & mask[7]) >> 7);
	*result2 = (*result2 << 1) +  (byteRepresentation[divided+1] & mask[0]);
      }
    else 
      {      
	nat tmp = (byteRepresentation[DIVIDE_8(pos)] & doubleMask[rest] ) >> rest; 
	*result1 = (*result1 << 1) + ( tmp & 1);  
	*result2 = (*result2 << 1) + (tmp >> 1); 
      }
  } 


  void initRandom(Randomness &rng);

private:
  uint32_t *startByGen;  	// yields the starting num 
  uint8_t *byteRepresentation; 
  nat numBytes; 

  static const uint8_t mask[8];
  static const uint8_t doubleMask[8];
  
  ChromConfig(const ChromConfig &rhs); 
  ChromConfig& operator=(const ChromConfig &rhs); 
}; 



////////////////////
// IMPLEMENTATION //
////////////////////

template<nat PLOIDY> ChromConfig<PLOIDY>::ChromConfig(nat numIndi, uint32_t *lengths, nat _startGen, nat endGen)
  : TimeSection(_startGen, endGen)
{
  startByGen = new nat[this->getNumGen()]; 
  
  startByGen[0] = 0; 
  nat startNow = lengths[0] * PLOIDY;
  for(nat i = this->getStartGen() + 1; i < endGen; ++i)
    { 
      startByGen[this->getGenIdx(i)] = startNow; 
      startNow += lengths[this->getGenIdx(i)] * PLOIDY; 
    }

  nat tmp = numIndi * PLOIDY;   

  numBytes =  DIVIDE_8(tmp)  + (MODULO_8(tmp) ? 1 : 0 ) ; 
  byteRepresentation = new uint8_t[numBytes]; 
}


template<nat PLOIDY> ChromConfig<PLOIDY>::~ChromConfig()
{
  delete [] startByGen; 
  delete [] byteRepresentation ;  
}


template<nat PLOIDY> void ChromConfig<PLOIDY>::initRandom(Randomness &rng)
{
  rng.initArray(this->byteRepresentation, this->numBytes); 
}


template<> inline nat ChromConfig<2>::getConfig(nat gen, haploAddr haploNr)
{
  nat pos = startByGen[this->getGenIdx(gen)] + haploNr;
  nat rest = MODULO_8(pos);
  return (byteRepresentation[DIVIDE_8(pos)] & mask[rest] ) >> rest; 
}

