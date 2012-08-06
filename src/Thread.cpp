#include "ThreadPool.hpp"
#include "Thread.hpp"


// no binding currently  
// #include "../ThreadBinding.hpp"
#define EVERY_SECOND [](nat tid){return tid * 2; }
// #define LINEAR [](nat tid){return tid; }

const std::function<nat(nat)> Thread::threadToCpu = EVERY_SECOND; 
// const std::function<nat(nat)> ThreadPool::threadToCpu = LINEAR;


Thread::Thread(nat _tid, nat seed, nat numEstimate, uint8_t* _barrierSlot, uint8_t* _finishSlot, pFun* _workFunction)
  : tid(_tid)
  , rng(seed)
  , freeRing(numEstimate)
  , barrierSlot(_barrierSlot)
  , finishSlot(_finishSlot)
  , workFunction(_workFunction)
  , mutationMemory(1000) 		// :MAGIC:
{
  // :TODO: use in multithreaded 
  // thread binding 
  // bindThreadToCacheGroup(tid);
  
  if(tid)
    this->thread_ptr = new thread(&Thread::workerLife, this, tid);   
}



void Thread::workerLife(nat id)
{ 
  cout << "this is worker thread " << id << endl; 

  while(1)
    {
      if(*barrierSlot == 0)
  	{
	  (*workFunction)(id); 
  	  *barrierSlot = 1; 
  	}
      
      if(*finishSlot)
  	break;       
    }
}


void Thread::bindThreadToCacheGroup(nat tid)
{  
  cpu_set_t mask; 
  int ret;   
  CPU_ZERO(&mask);   

  nat toBind = threadToCpu(tid); 
  CPU_SET(toBind , &mask); 
  // cout << "pinning thread "  << tid << " to " <<  toBind  << endl; 

  ret = sched_setaffinity(0, sizeof(mask), &mask); 
  if(ret)
    {      
      cout << "something went wrong binding thread " << tid << " error code " << ret << endl; 
      abort();
    }
}

