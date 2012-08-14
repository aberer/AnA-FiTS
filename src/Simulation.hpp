#pragma once 

#include "common.hpp"
#include "InfoFile.hpp"
#include "types.hpp"
#include "FractionalSimulation.hpp"

// simulation should take care of the top-level stuff, e.g. memory manangement  
//  or the random ressource 

class ProgramOptions; 

class Simulation
{  
public: 
  Simulation(InfoFile &info, ProgramOptions &theProgOpt);   
  void run(); 

private: 
  ProgramOptions &progOpt;
  nat numGen; 
  ThreadPool tp; 
  FractionalSimulation *fractionalSimulation;
  vector<Chromosome*> chromosomes;
  vector<PopulationParams> popParams;   
}; 
