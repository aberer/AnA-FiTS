#include "ThreadPool.hpp"

#include <cstdlib>
#include <iostream>
#include <cassert>


ThreadPool::ThreadPool(nat _numThreads, nat randSeed)  
  : numThreads(_numThreads)
  , threads(numThreads)
  , loadBalancer(50)
{
  // parallelity 
  barrier = new byte[numThreads];
  fill(barrier, barrier + numThreads, 1 ); 
  workerFunction = (pFun**)malloc(numThreads * sizeof(pFun*)); 
  finish = (byte*) calloc(numThreads, sizeof(byte)); 

  // cout << "this is the master thread " << endl; 

  // initialize the threads 
  auto seqEstimate = 1000; // :MAGIC: 
  threads[0] = new Thread(0,randSeed, seqEstimate, barrier, finish, workerFunction[0]);
  Randomness &rng = threads[0]->getRNG(); 
  for(nat i = 1 ; i < numThreads; ++i)
    threads[i] = new Thread(i, rng.Integer(), seqEstimate, barrier + i, finish + i, workerFunction[i]);  
}


ThreadPool::~ThreadPool()
{
  delete [] barrier; 
  free(workerFunction) ; 
  free(finish);   
  for(nat i = 0; i < getNumberOfThreads();++i)
    delete threads[i]; 
}


void ThreadPool::joinThreads()
{
  for(nat i = 1; i < numThreads; ++i)
    finish[i] = 1; 

  for(nat i = 1; i < numThreads; ++i)
    threads[i]->join();
}
