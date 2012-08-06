#pragma once 

#include "common.hpp"
#include "types.hpp"

class TimeSection
{
public: 
  TimeSection(nat _startGen, nat _endGen); 

  // ACCESSORS 
  nat getNumGen() const { return endGen-startGen; }
  nat getStartGen() const {return startGen; }
  nat getEndGen() const {return endGen; }
  nat getGenIdx(nat idx) const { return idx - startGen; }

private: 
  nat startGen; 
  nat endGen; 
}; 

