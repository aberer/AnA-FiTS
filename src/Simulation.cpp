#include "Simulation.hpp"
#include "ProgramOptions.hpp"
#include "ThreadPool.hpp"

#include <fstream>
#include <vector>


Simulation::Simulation(InfoFile &info, ProgramOptions &theProgOpt)   
  : progOpt(theProgOpt) 
  , numGen(progOpt.getNumberOfGenerations())
  , tp(1, progOpt.get<nat>("seed"))
{     

  // create chromosomes 
  Randomness &rng = tp[0].getRNG();
  vector<seqLen_t> lengths = progOpt.get<vector<seqLen_t> >("length");  
  nat idC = 0; 

  nat numPop = 1; 		// TODO pops 
  
  for(auto& length : lengths)
    {
      FitnessFunction fFun(progOpt.get<vector<string>>("selCoef"));
      Chromosome* chromo = new Chromosome(length, theProgOpt.hasOption("neutral"), fFun, idC++, numPop);
      chromo->init(rng);
      chromosomes.push_back(chromo); 
    }

  // create parameters of populations 
  vector<nat> popSizes = progOpt.get<vector<nat> >("popSize"); 
  assert(popSizes.size() == 1 );
  popParams.push_back( PopulationParams(0, numGen, popSizes[0], progOpt));  

  fractionalSimulation = new FractionalSimulation(tp,info,  progOpt, numGen, chromosomes, popParams); 
}


Simulation::~Simulation()
{
  delete fractionalSimulation; 
}


void Simulation::run()
{     
#ifdef DEBUG_TOPLEVEL
  cout << "number of generations:  " <<  numGen << endl; 
#endif

  fractionalSimulation->simulate();
  fractionalSimulation->finalize();

  string runid = progOpt.get<string>("runId"); 
  fractionalSimulation->printSequencesRaw(runid); 
  fractionalSimulation->printArgs(runid);

  tp.joinThreads();
}
