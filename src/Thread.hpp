#pragma once 

#include "Randomness.hpp"
#include "FreeRing.hpp"
#include "ResizMemArena.hpp"

#include <thread>


class Thread
{
public: 
  Thread(nat _tid, nat seed, nat numEstimate, uint8_t* _barrierSlot, uint8_t* _finishSlot, pFun* _workFunction); 

  // ACCESSORS
  Randomness& getRNG(){return rng; }
  FreeRing& getFreeRing(){return freeRing;}
  ResizMemArena<SelectedMutation>& getMutationMemory() {return mutationMemory; }
  void join(){thread_ptr->join();}


private:
  nat tid; 
  Randomness rng; 
  FreeRing freeRing; 
  uint8_t *barrierSlot;
  uint8_t *finishSlot; 
  pFun *workFunction; 
  thread *thread_ptr;   
  static const std::function<nat(nat)> threadToCpu; 
  ResizMemArena<SelectedMutation> mutationMemory; 

  void workerLife(nat id);   
  void bindThreadToCacheGroup(nat tid);  
  Thread& operator=(const Thread &rhs); 
  Thread(const Thread &rhs); 
}; 
