#include "../src/Ancestry.hpp"
#include <gtest/gtest.h>






// TEST(AncestryTest, general)
// {
//   nat startGen = 0; 
//   nat endGen = 10; 
//   nat popSize = 20; 
//   nat haploSize = 2 * popSize; 
//   nat seqLen = 1000; 
//   float mutRate = 0.0001; 
//   float recRate = 0.0001; 

//   PopulationParams popParam(popSize, recRate, mutRate); 
//   vector<PopulationParams> popParams; 
//   popParams.push_back(popParam); 
//   PopulationManager popMan(popParams);   
//   ThreadPool tp(1,123);  
//   FitnessFunction fFun(vector<string>({"2", "0.0", "10", "400", "0.0", "1", "20"}));  

//   Chromosome chrom(seqLen, fFun, 0);
//   chrom.getRecombinationManager().precompute(tp,popParams,startGen,endGen);

  
//   cout << chrom.getRecombinationManager() << endl; 

//   vector<Chromosome*> chromosomes; 
//   chromosomes.push_back(&chrom); 
  
//   // initialize ancestry 
//   Ancestry ancestry(popParams, endGen-startGen, chromosomes);
//   ancestry.fillWithRandomIndividuals(tp);
  
//   float* bla = (float*)malloc(100 * sizeof(float)); 
//   for(nat i = 0; i < 100 ; ++i)
//     bla[i] = 1.0; 

//   for(nat i = startGen; i < endGen - startGen; ++i )
//     ancestry.resampleParentsByFitness<uint8_t>(tp, i, bla); 

//   cout << ancestry << endl; 

//   Graph &graph = ancestry.getGraph(0);
//   Survivors survivors(haploSize, endGen-startGen) ; 

//   ancestry.updateGraph_inner(tp, survivors, chrom, popMan, graph,  haploSize); 
  
//   cout << "PRINTING SURVIVORS" << endl; 
//   cout << survivors << endl; 
//   cout << "DONE" << endl; 

//   cout << "HOOKING UP GRAPH" << endl; 
//   graph.hookup(survivors, ancestry, popMan, 0);   
//   cout << "DONE" << endl; 

//   cout << "BACKTRACING SEQS" << endl; 
//   vector<NeutralArray*> seqs; 
//   graph.backtrace(seqs, chrom);
//   cout << "DONE" << endl;
// }



// TEST(AncestryTest, generalLong)
// {
//   nat startGen = 0; 
//   nat endGen = 30; 
//   nat popSize = 10; 
//   nat haploSize = 2 * popSize; 
//   nat seqLen = 1000; 
//   float mutRate = 0.0001; 
//   float recRate = 0.0001; 

//   PopulationParams popParam(popSize, recRate, mutRate); 
//   vector<PopulationParams> popParams; 
//   popParams.push_back(popParam); 
//   PopulationManager popMan(popParams);   
//   ThreadPool tp(1,123);  
//   FitnessFunction fFun(vector<string>({"2", "0.0", "10", "400", "0.0", "1", "20"}));  

//   Chromosome chrom(seqLen, fFun, 0);
//   chrom.getRecombinationManager().precompute(tp,popParams,startGen,endGen);

  
//   cout << chrom.getRecombinationManager() << endl; 

//   vector<Chromosome*> chromosomes; 
//   chromosomes.push_back(&chrom); 
  
//   // initialize ancestry 
//   Ancestry ancestry(popParams, endGen-startGen, chromosomes);
//   ancestry.fillWithRandomIndividuals(tp);
  
//   FITNESS_TYPE* bla = (FITNESS_TYPE*)malloc(100 * sizeof(FITNESS_TYPE)); 
//   for(nat i = 0; i < 100 ; ++i)
//     bla[i] = 1.0; 

//   for(nat i = startGen; i < endGen - startGen; ++i )
//     ancestry.resampleParentsByFitness<uint8_t>(tp, i, bla); 

//   cout << ancestry << endl; 

//   // Graph &graph = ancestry.getGraph(0);
//   Graph graph(1000);
//   Survivors survivors(haploSize, 10000,endGen-startGen) ; 

//   ancestry.updateGraph_inner(tp, survivors, chrom, popMan, graph,  haploSize); 
  
//   cout << "PRINTING SURVIVORS" << endl; 
//   cout << survivors << endl; 
//   cout << "DONE" << endl; 

//   cout << "HOOKING UP GRAPH" << endl; 
//   graph.hookup(survivors, ancestry, popMan, chrom, tp[0].getRNG());   
//   cout << "DONE" << endl; 

//   graph.createSequencesInGraph(chrom);
// }
