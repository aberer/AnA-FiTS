#pragma once


#include "common.hpp"

// #include "../lib/sigar/sigar.h"

#include <sys/resource.h>

class System
{
  int who; 
  // sigar_pid_t ownPid; 
  struct rusage usage;
  // sigar_t *sigar;

public: 
  System();
  ~System();

  void process(char *when);
  double getMaxMemConsumption();   
  void getFreeMem(); 
  void getProcMem(); 
  long getTotalSystemMemory(); 
}; 
