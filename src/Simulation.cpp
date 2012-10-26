#include "Simulation.hpp"
#include "ProgramOptions.hpp"
#include "BinaryWriter.hpp"

#include <fstream>
#include <vector>


Simulation::Simulation(InfoFile &info, ProgramOptions &theProgOpt, ThreadPool &_tp)
  : progOpt(theProgOpt) 
  , numGen(progOpt.getNumberOfGenerations())
  , tp(_tp)
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



#include <sstream>

void Simulation::run()
{     
#ifdef DEBUG_TOPLEVEL
  cout << "number of generations:  " <<  numGen << endl; 
#endif
  
  fractionalSimulation->simulate();
}

void Simulation::printResult()
{
  // we are doing this on a chromosome basis, in order to keep the
  // memory consumption down for a large number of chromosomes

  string runid = progOpt.get<string>("runId");   
  stringstream fileName; 
  nat numChrom = chromosomes.size(); 
  fileName << SEQ_FILE_NAME << "." << runid ; 
  BinaryWriter seqWriter(fileName.str(),numChrom,0);
  
  stringstream fn2 ; 
  fn2 << ARG_FILE_NAME  << "." << runid; 
  BinaryWriter graphWriter(fn2.str(), 0,numChrom); 

  for(nat i = 0; i < numChrom; ++i)
    {
      fractionalSimulation->finalize(i);
      Graph *graph = fractionalSimulation->getGraphHandle(i);      
      Chromosome *chrom = chromosomes[i]; 
      HaploTimeWindow *haplo = fractionalSimulation->getHaploWindowHandle(i); 
      seqWriter.writeSequences(*graph, *haplo, *chrom );
      graphWriter.writeGraph(*graph);
      fractionalSimulation->deleteGraph(i); 
    }
 
}
