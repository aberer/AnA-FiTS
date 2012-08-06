#pragma once 
#include "common.hpp"
#include <cstdint>
#include "../lib/cycle.hpp"



class CycleCounter
{
  uint64_t accumulatedCycles;
  ticks lastStart; 
  
public: 
  CycleCounter() 
  : accumulatedCycles(0), lastStart(0)
  {} 
  

  void resetCycles()
  {
    accumulatedCycles = 0; 
    lastStart = getticks();
  }
  
  void start()  {    lastStart = getticks();  }

  void stop()
  {
    ticks t2  = getticks();
    accumulatedCycles += elapsed(t2, lastStart); 
  }

  uint64_t getElapsed()
  {
    stop();
    return accumulatedCycles; 
  }
}; 
