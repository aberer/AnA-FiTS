#pragma once 

#include "common.hpp"
#include "types.hpp"
#include "Randomness.hpp"
#include "LoadBalancer.hpp"
#include "Thread.hpp"

#include <thread>
#include <vector>
#include <mutex>
#include <cstring>


class ThreadPool
{  
public: 
  // LIFE CYCLE 
  ThreadPool(nat _numThreads, nat randSeed); 
  ~ThreadPool(); 

  
  // OPERATORS 
  void masterBarrier(pFun *workFunction);
  void joinThreads();
  nat getNumberOfThreads(){return numThreads; }  
  void acquirePrintLock(){printMutex.lock();} 
  void releasePrintLock(){printMutex.unlock();}

  // ACCESSORS
  LoadBalancer& getLoadBalancer() { return loadBalancer; } 
  void setNumJobs(nat numJobs){loadBalancer.reset(); loadBalancer.setNumberOfJobs(numJobs);}
  Thread& operator[](nat num){return *(threads[num]);}
  
private:   
  nat numThreads;
  vector<Thread*> threads; 
  pFun **workerFunction; 

  byte *barrier;
  byte *finish; 
  mutex printMutex;  

  LoadBalancer loadBalancer;

  ThreadPool(const ThreadPool &rhs); 
  ThreadPool& operator=(const ThreadPool &rhs);   
};



inline void ThreadPool::masterBarrier(pFun *workFunction)
{
  nat sum = 0;  
  fill (workerFunction,workerFunction + numThreads, workFunction  ) ; 
  memset(barrier, 0, numThreads * sizeof(byte)); 

  // do your own work
  (*workerFunction[0])(0); 
  barrier[0] = 1; 
  
  // master waits for workers 
  do
    {
      sum = 0; 
      for(nat i = 0; i < numThreads; ++i)
  	sum += barrier[i];
    } while(sum < numThreads); 
  
  loadBalancer.reset();
}
