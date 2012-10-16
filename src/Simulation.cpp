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


void Simulation::run()
{     
#ifdef DEBUG_TOPLEVEL
  cout << "number of generations:  " <<  numGen << endl; 
#endif

  fractionalSimulation->simulate();
  fractionalSimulation->finalize();
  
#ifdef VERIFICATION
  vector<Graph*> graphs = fractionalSimulation->getGraphs();
  assert(graphs.size() == 1);
  Graph* myGraph = graphs[0];
  vector<Node*> state = myGraph->getState();
  vector<Chromosome*> chrs =  fractionalSimulation->getChromosomes();

  vector<NeutralArray*> slowSequences; 
  myGraph->getSequencesSlow(slowSequences, *(chrs[0]));

  vector<NeutralArray*> fastSequences; 
  for(int i = 0; i < 1000; ++i)
    {
      Node *node = state[i]; 
      NeutralArray *seq = myGraph->getSequenceFromNode(node);
      fastSequences.push_back(seq); 
    }

  assert(fastSequences.size() == slowSequences.size()); 

  for(int i = 0; i < slowSequences.size(); ++i)
    {
      cout << "slow: "  << *(slowSequences[i]) << endl; 
      cout << "fast: " << *(fastSequences[i]) << endl; 
      cout << endl; 
    }  
#endif

  string runid = progOpt.get<string>("runId"); 
  fractionalSimulation->printSequencesRaw(runid); 
  fractionalSimulation->printArgs(runid);

  tp.joinThreads();
}
