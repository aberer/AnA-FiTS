#include "FractionalSimulation.hpp"
#include "ProgramOptions.hpp"
#include "SequenceArray.hpp"
#include "Ancestry.hpp"
#include "BinaryWriter.hpp"

#include  <fstream>
#include <functional>
#include <algorithm>

FractionalSimulation::FractionalSimulation(ThreadPool &_tp,InfoFile &info, const ProgramOptions &progOpt, nat _numGen, vector<Chromosome*> _chromosomes, vector<PopulationParams> popParams) 
  : genCnt(_numGen, ( progOpt.hasOption("memory") ? progOpt.get<string>("memory") : "" ) )
  , numSelMut(0) 
  , cleanFixPeriod(progOpt.get<nat>("cleanF"))
  , tp(_tp) 
  , chromosomes(_chromosomes)
  , isNeutral(false)
{
  popMan = unique_ptr<PopulationManager>(new PopulationManager(popParams)); 

  SelectedArray *selectedInit = new SelectedArray(100); // MAGIC 
  init = selectedInit; 
  for(auto chromo : chromosomes)
    {
      haplotypesWindows.push_back(new HaploTimeWindow (popParams[0].getPopSizeByGeneration(0), selectedInit));       
      graphs.push_back(new Graph(MULT_2(popParams[0].getPopSizeByGeneration(0)))); // :MAGIC: :KLUDGE:      
    }

  if(progOpt.hasOption("neutral"))
    isNeutral = true; 

  info.write("\t\t\t\tGENERAL INFO\n"); 
  info.write("\t\tinitial ancestrial population size %d\n", popMan->getTotalNumIndiByGen(0)); 
  
  for(nat i = 0; i < chromosomes.size(); ++i)
    {
      info.write("\t\tCHROM %d (length=%d) gen1 pop1\t=> E(non-neut mutations) = %g\tE(neut mutations) = %g\tE(recomb) = %g\n" ,
		 i , chromosomes[i]->getSeqLen(), 
		 (PopulationManager::getLamdbaForParam( (*popMan)[0].getMutationRate(0), chromosomes[i]->getSeqLen(), *popMan, 0, 0) *  (isNeutral ?  0 : chromosomes[i]->getSelectProb())),
		 (PopulationManager::getLamdbaForParam( (*popMan)[0].getMutationRate(0), chromosomes[i]->getSeqLen(), *popMan, 0, 0) * (isNeutral ? 1 : chromosomes[i]->getNeutralProb() )), 
		 (PopulationManager::getLamdbaForParam((*popMan)[0].getRecombinationRate(0), chromosomes[i]->getSeqLen(), *popMan, 0, 0 ))); 
    }
}


FractionalSimulation::~FractionalSimulation()
{
  for(nat i = 0; i < chromosomes.size(); ++i)
    delete chromosomes[i] ; 
  for(HaploTimeWindow *haplo : haplotypesWindows)
    delete haplo; 
  delete init; 
}


// TODO check, if parents selfs (which is not possible)
// TODO selfing 
// :NOTICE: Linkage Could Be represented here 
void FractionalSimulation::sampleParentsByFitness(ThreadPool &tp)
{
  nat currentGen = genCnt.getCurrentGeneration();
  nat numHaploPrev = popMan->getTotalNumIndiByGen(currentGen);

#ifdef DEBUG_SHOW_POPSIZE
  cout << "gen " << genCnt.getCurrentGeneration() << "\t" << numHaploPrev  << endl; 
#endif

  // resample 
  if(isNeutral)
    {
      CHOOSE_IMPLEMENTATION_BY_TYPE_AND_NEUTRALITY(numHaploPrev, true, ancestry->resampleParentsByFitness, tp, *popMan, currentGen);
    }
  else 
    {
      CHOOSE_IMPLEMENTATION_BY_TYPE_AND_NEUTRALITY(numHaploPrev, false, ancestry->resampleParentsByFitness, tp, *popMan, currentGen);
    }    

  // propagate the haplotypes
  Ancestry &ancst = *ancestry; 

  nat numChrom =  chromosomes.size(); 
  for(nat i = 0; i < numChrom; ++i)
    {
      HaploTimeWindow &haplo = *(haplotypesWindows[i]); 
      CHOOSE_IMPLEMENTATION_BY_TYPE(numHaploPrev, haplo.propagate, currentGen, ancst, i, *popMan);
    }
}


void FractionalSimulation::createRecombinants(ThreadPool &tp)
{
  nat currentGen = genCnt.getCurrentGeneration();
  FreeRing &fr = tp[0].getFreeRing();

  nat numChrom = chromosomes.size(); 
  for(nat chromId = 0; chromId < numChrom; ++chromId)
    {
      // TODO for all populations 
      RecombinationManager &recMan = ancestry->getRecombinationManager(chromId);
      HaploTimeWindow &haplos = *(haplotypesWindows[chromId]);   
      nat numRec =  recMan.getNumRecByGen(currentGen);

#ifdef DEBUG_RECOMBINATION
      cout << "number rec " << numRec << " for chrom " << chromId  << endl;  
#endif

      nat i = 0; 
      while(i < numRec)
	{
	  nat length = 0; 
	  
	  Recombination *start = nullptr;
	  recMan.obtainRecsForIndividual(start, length);
	  
	  if(NOT start)
	    {
	      assert(length == 0); 
	      break; 
	    }

	  
	  // necessary, because obtainRecsForIndividual sorts the recombinations
	  if(isNeutral)
	    {
	      i += length; 
	      continue; 
	    }
	  
	  nat ancestorA = ancestry->getAddrOfParent(currentGen, chromId,start->haploIndiNr); 
	  SelectedArray *anc1 = haplos.getPreviousConfiguration(ancestorA),
	    *anc2 = haplos.getPreviousConfiguration(GET_OTHER_ANCESTOR(ancestorA));	  

	  assert( genCnt.getCurrentGeneration() == 0 ||  ancestorA < popMan->getTotalNumHaploByGen(genCnt.getCurrentGeneration()-1)); 

	  if(anc1 != anc2)
	    {
	      SelectedArray *recombinant = fr.allocate(currentGen);
	      recombinant->erase();

	      assert(recombinant != anc1 && recombinant != anc2); 
	      recombinant->recombine(*anc1,*anc2, start, length); 

	      haplos.setCurrentConfiguration(start->haploIndiNr, recombinant);
	      recombinant->computeFitness();
	    }

	  i += length; 
	}
    }
}


void FractionalSimulation::updateFitness()
{      
  nat numChrom = chromosomes.size(); 
  nat currentGen = genCnt.getCurrentGeneration();

  nat totalSize = popMan->getTotalNumIndiByGen(currentGen); 
  FITNESS_TYPE *fitnessValues = popMan->getFitnessValues(0); 
  fitnessValues = (FITNESS_TYPE*) myRealloc(fitnessValues, totalSize * sizeof(FITNESS_TYPE)); 
  FITNESS_TYPE maxF = 0.;
  
  for(nat i = 0; i < totalSize; ++i)
    {      
      // cout << i << endl; 
      FITNESS_TYPE fitness = 1.; 
      for(nat c = 0; c < numChrom; ++c)
	{
	  HaploTimeWindow &haplos = *(haplotypesWindows[c]);

	  nat haploAddr = MULT_2(i);
	  SelectedArray *seqA =  haplos.getCurrentConfiguration(haploAddr) ; 
	  SelectedArray *seqB =  haplos.getCurrentConfiguration(haploAddr+1) ; 

	  FITNESS_TYPE a = seqA->getFitness(); 
	  FITNESS_TYPE b = seqB->getFitness(); 

	  assert(a > 0 && b > 0 && fitness > 0); 

	  fitness *=  a * b;
	}

      fitnessValues[i] = fitness; 
    }
  
  maxF = *(max_element(fitnessValues, fitnessValues + totalSize ));   

#ifdef DEBUG_FITNESS
  cout << "maximum fitness is "  << maxF << " and noramlizing is " << 1.0 / maxF << endl; 
#endif

  maxF = 1.0 / maxF; 

  for(nat i = 0; i < totalSize; ++i)
    fitnessValues[i] *= maxF; 

  popMan->setFitnessValues(0, fitnessValues);
}


void FractionalSimulation::mutateSelectedSites(ThreadPool &tp)  
{
  if(isNeutral)
    return; 

  nat currentGen = genCnt.getCurrentGeneration();
  nat numChrom = chromosomes.size(); 
  for(nat chromId = 0; chromId < numChrom; ++chromId)
    {     
      Chromosome &chromosome = *(chromosomes[chromId]);
      HaploTimeWindow &haplos = *(haplotypesWindows[chromId]); 
      Randomness &rng = tp[0].getRNG(); 
      FreeRing &fr = tp[0].getFreeRing();
      ResizMemArena<SelectedMutation> &mem =  tp[0].getMutationMemory(); 
  
      nat currentHaploNum = popMan->getTotalNumHaploByGen(currentGen); 
      
      float lambda = PopulationManager::getLamdbaForParam((*popMan)[0].getMutationRate(currentGen), chromosome.getSeqLen(), *popMan, 0, currentGen) * chromosome.getSelectProb() ; 
      nat numMut = rng.samplePoisson(lambda);       

      numSelMut += numMut;  
      
#ifdef DEBUG_MUT_SEL
      cout << "number of mutations " << numMut  << " in chrom " << chromId << endl; 
#endif
  
      for(nat c = 0; c < numMut; ++c)
	{      
	  nat mutNum = rng.Integer(currentHaploNum); 
	  SelectedArray *tmp = haplos.getCurrentConfiguration(mutNum); 

	  if(tmp->getCreationGen() != currentGen)
	    {
	      SelectedArray *ptr = tmp; 
	      tmp = fr.allocate(currentGen); 
	      *tmp = *ptr; 
	    }

	  SelectedMutation &mut = chromosome.getSelectedMutation(mem, rng);
	  mut.generation = currentGen; 
	  mut.indiNr = mutNum; 
	  tmp->mutate(mut, true);

	  haplos.setCurrentConfiguration(mutNum, tmp); 
	}
    }
}


void FractionalSimulation::simulate() 
{
  while(genCnt.hasToSimulate())
    {
      genCnt.determineNextSection(*popMan, chromosomes);
      // cout << "GenerationCounter: simulating from " << genCnt.getStartOfSection() << " until " << genCnt.getEndOfSection() << endl; 
      
      // set up ancestry 
      ancestry = new Ancestry(tp, chromosomes, genCnt, *popMan); 
      ancestry->fillWithRandomIndividuals(tp); 

      // simulate this part, then update graph 
      while(genCnt.hasNext())
	{
	  nat currentGen = genCnt.getCurrentGeneration();
#ifdef DEBUG_SHOW_GENS
	  if(NOT (currentGen % (genCnt.getTotalNumGen() / 10 )) )
	    cout << "**************** entering generation " << currentGen << "****************   " << (double(currentGen) / double(genCnt.getTotalNumGen()) ) * 100    << " %" << endl; 
#endif
      
	  sampleParentsByFitness(tp);
	  createRecombinants(tp);
	  mutateSelectedSites(tp);
      
	  if(currentGen > 1 && NOT (currentGen % cleanFixPeriod) )
	    cleanFixed();
      
	  updateFitness();
      
	  for(auto &h : haplotypesWindows)
	    h->switchPastAndPresent();
      
	  genCnt.next();
	}
      
      nat numChrom = chromosomes.size(); 
      for(nat i = 0; i < numChrom; ++i)
	ancestry->updateGraph(tp, *(chromosomes[i]), *(graphs[i]), *popMan, genCnt);

      delete ancestry; 
    }
}


void FractionalSimulation::printArgs(string id)
{  
  stringstream fileName; 
  fileName << ARG_FILE_NAME  << "." << id ; 
  FILE *fh = openFile(fileName.str(), "w"); 

  nat numChrom = chromosomes.size(); 
  BIN_WRITE(numChrom,fh); 
  nat c = 0;
  for(nat i = 0; i < numChrom; ++i)
    {
      Graph &graph = *(graphs[i]);
      graph.printRaw(fh); 
    }
}


void FractionalSimulation::printSequencesRaw(string id)
{
  nat numHaplo = popMan->getTotalNumHaploByGen(genCnt.getCurrentGeneration() - 1); 
  BinaryWriter writer(id, numHaplo);

  nat numChrom = chromosomes.size();  
  writer.writeInt(numChrom); 
  for(nat i = 0; i < numChrom; ++i)
    {
      Graph &graph = *(graphs[i]);
      HaploTimeWindow &w = *(haplotypesWindows[i]);
      Chromosome &chrom = *(chromosomes[i]); 
      writer.write(graph, w, chrom); 
    }
}


void FractionalSimulation::finalize()
{
  nat parNum = popMan->getTotalNumHaploByGen(genCnt.getCurrentGeneration() -1); 

  for(nat i = 0; i < haplotypesWindows.size(); ++i)
    {
      HaploTimeWindow &h  = *(haplotypesWindows[i]) ;
      h.switchPastAndPresent();
      
      auto curSeqIter = h.start();
      chromosomes[i]->cleanFixedMutations(curSeqIter, curSeqIter + parNum, 0, tp); // :TODO: for all popultaions
    }

  // neutral part 
  nat numChrom = chromosomes.size();
  for(nat i = 0; i < numChrom; ++i)
    {
      Graph &graph = *(graphs[i]); 
      if( graph.getNumberOfMutations())
	graph.createSequencesInGraph(*(chromosomes[i]));
      else 
	cerr << "NOTICE: no neutral mutations can occur (because of -W), no graph produced." << endl; 
    }
}


inline void FractionalSimulation::cleanFixed()
{
  nat parNum = popMan->getTotalNumHaploByGen(genCnt.getCurrentGeneration());

  // reset mutation references 
  nat numChrom = chromosomes.size();
  for(nat i = 0; i < numChrom; ++i)    
    {
      auto begin = haplotypesWindows[i]->start(); 
      chromosomes[i]->cleanFixedMutations(begin , begin + parNum, 0 , tp ); // :TODO: for all populations
    }
}
