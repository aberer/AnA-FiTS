#pragma once 

#include "common.hpp"
#include "HaploTimeWindow.hpp"
#include "PopulationManager.hpp"
#include "Ancestry.hpp"
#include "GenerationCounter.hpp"
#include "PopulationParams.hpp"
#include "ThreadPool.hpp"

class ProgramOptions; 

// :CONVENT: we start simulation in generation 0  ; there is a virtual generation preceeding this generation 

class FractionalSimulation 
{
public: 
  FractionalSimulation(ThreadPool &tp,const ProgramOptions &progOp, nat numGen, vector<Chromosome*> chromosomes, vector<PopulationParams> popParams); 
  ~FractionalSimulation();
  void simulate(); 
  void printSequencesRaw(string id); 
  void printArgs(string id ); 
  void finalize(); 

private:
  vector<HaploTimeWindow*> haplotypesWindows;  // PER chromosome 
  unique_ptr<PopulationManager> popMan;
  GenerationCounter genCnt; 
  Ancestry* ancestry;   
  nat numSelMut;
  nat cleanFixPeriod; 
  SelectedArray *init;  
  ThreadPool &tp; 
  vector<Graph*> graphs;
  vector<Chromosome*> chromosomes;

  // METHODS
  void sampleParentsByFitness(ThreadPool &tp);   
  void createRecombinants(ThreadPool &tp); 
  void updateFitness();
  void mutateSelectedSites(ThreadPool &tp); 
  SelectedArray *getSequenceArray();
  void cleanFixed();
  
  // COPY 
  const FractionalSimulation& operator=(const FractionalSimulation &rhs);
  FractionalSimulation(const FractionalSimulation &rhs); 

  friend class PopulationTest;
};



////////////////////
// IMPLEMENTATION //
////////////////////


