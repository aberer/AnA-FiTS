#include "Ancestry.hpp"

#include "GenerationCounter.hpp"


Ancestry::Ancestry(ThreadPool &tp, vector<Chromosome*> chromosomes, GenerationCounter &genCnt, const PopulationManager &popMan) 
  : TimeSection(genCnt.getStartOfSection(), genCnt.getEndOfSection())
  , genStartIndi(new uint8_t*[this->getNumGen()])
  , length(new uint32_t[this->getNumGen()])
  , numChrom(chromosomes.size())
  , chromConfig(chromosomes.size())

{ 
  initNum = genCnt.getStartOfSection() == 0 ? popMan.getTotalNumHaploByGen(0) : popMan.getTotalNumHaploByGen(genCnt.getStartOfSection()-1);  

  nat totalNumIndi = initMemBlock(popMan);
  for(nat i = 0; i < numChrom;++i )
    chromConfig[i] = new ChromConfig<2>(totalNumIndi, this->length, genCnt.getStartOfSection(), genCnt.getEndOfSection());

  // precompute recombinations 
  for(nat i = 0; i < numChrom; ++i)
    {
      recMans.push_back(new RecombinationManager(chromosomes[i]->getSeqLen(), genCnt.getStartOfSection(), genCnt.getEndOfSection())); 
      recMans[i]->precompute(tp, popMan);
    }
}


Ancestry::~Ancestry()
{
  delete [] genStartIndi; 
  delete [] length; 
  for(nat i = 0; i < numChrom; ++i)
    delete chromConfig[i];  
  free( memoryBlockIndi );
  for(auto rec : recMans)
    delete rec ; 
}


nat Ancestry::initMemBlock(const PopulationManager &popMan)
{
  // determine number of individuals 
  nat totalNumIndi = 0;
  size_t totalByteNeeded = 0;   

  nat endGen = this->getEndGen();
  for(nat i = this->getStartGen(); i < endGen; ++i)
    { 
      nat numIndi = popMan.getTotalNumHaploByGen(i);
      length[getGenIdx(i)] = numIndi; 
      totalNumIndi += numIndi; 
      totalByteNeeded += popMan.getNumBytesForGeneration(i); 
      assert(NOT MODULO_16(totalByteNeeded)); 
    }

  // reserve mem block 
  memoryBlockIndi = (uint8_t*) malloc_aligned(totalByteNeeded, 16); 

  // initialize offset-array
  genStartIndi[0] = memoryBlockIndi; 
  for(nat i = this->getStartGen() + 1 ; i < endGen; ++i)
    genStartIndi[this->getGenIdx(i)] = genStartIndi[this->getGenIdx(i-1)] + popMan.getNumBytesForGeneration(i); 

  return totalNumIndi; 
}


#ifdef EFFICIENCY_INIT_MEMBLOCK
void Ancestry::fillWithRandomIndividuals_parallel(ThreadPool &tp, nat tid)
{
  nat numGen = this->getNumGen();  
  Randomness &rng = tp[tid].getRNG();  
  nat perThread = numGen / tp.getLoadBalancer().getTotalJobs();  
  // cout << "one job consists of " << perThread << " generations" << endl; 
  
  nat ticket; 
  while(tp.getLoadBalancer().getTicket(ticket))
    {
      nat start = ticket * perThread ; 
      nat end  = tp.getLoadBalancer().isLast(ticket) ? numGen : (ticket + 1 ) * perThread;

      for(nat i = start; i < end; ++i)
      	{
      	  nat numInPrev = (i == 0) ? DIVIDE_2(length[0]) : DIVIDE_2(length[i-1]);
	  uint32_t lengthHere = length[i];
	  
      	  if(numInPrev <= numeric_limits<uint8_t>::max())
	    rng.IntegerArray<uint8_t>(static_cast<uint8_t*>(this->genStartIndi[i]), lengthHere, numInPrev); 
      	  else if(numInPrev <= numeric_limits<uint16_t>::max()) 
	    rng.IntegerArray<uint16_t>(reinterpret_cast<uint16_t*>(this->genStartIndi[i]), lengthHere, numInPrev);
      	  else if(numInPrev <= numeric_limits<uint32_t>::max())
	    rng.IntegerArray<uint32_t>(reinterpret_cast<uint32_t*>(this->genStartIndi[i]), lengthHere, numInPrev); 
      	  else
      	    assert(0);

	  // cout << "initialized " << i << endl; 
      	}
    }
}
#else 
// void Ancestry::fillWithRandomIndividuals_parallel(ThreadPool &tp, nat tid)
// {
//   cout << "alternative sampling"   << endl; 
  
//   assert(0); 

//   Randomness &rng = tp[tid].getRNG();
//   nat perThread = numGen / tp.getLoadBalancer().getTotalJobs();  

//   for(nat i = 0; i < numGen; ++i)
//     {
//       nat numInPrev = (i == 0) ? DIVIDE_2(initNum) : DIVIDE_2(length[i-1]); 
//       uint32_t lengthHere = length[i]; 
      
//       if(i < numGen-1)
// 	assert(genStartIndi[i] + 2 * length[i] <= genStartIndi[i+1] );


//       assert(numInPrev > numeric_limits<uint8_t>::max() && numInPrev < numeric_limits<uint16_t>::max()); 

//       uint16_t* array = reinterpret_cast<uint16_t*>(this->genStartIndi[i]); 
//       for(nat j = 0; j < lengthHere; ++j)
// 	array[j] = rng.Integer(numInPrev); 
//     }    
// }
#endif


void Ancestry::fillWithRandomIndividuals(ThreadPool &tp)
{
  pFun func = [&](nat id) { fillWithRandomIndividuals_parallel(tp, id); }; 
  tp.masterBarrier(&func); 
  
  for(ChromConfig<2> *cc : chromConfig)
    cc->initRandom(tp[0].getRNG());
}



void Ancestry::insertNeutralMutations(ThreadPool& tp,  Graph &graph, const Survivors &survivors, const PopulationManager &popMan, Chromosome &chrom , const RegionManager &regMan, const nat genNum ) 
{
  Randomness &rng = tp[0].getRNG(); 
  nat *start = survivors.getFirstSurvivorForward(genNum); 
  nat *end = survivors.getFirstSurvivorForward(genNum-1);
  
  nat seqLen = chrom.getSeqLen();
  nat numSurvivors = end - start; 

  // :TODO: iterate through pops   
  float lambda = PopulationManager::getLamdbaForParam(popMan[0].getMutationRate(genNum), seqLen, popMan, 0, genNum); 
  float lambdaSmall = lambda * numSurvivors  / popMan.getTotalNumHaploByGen(genNum); 

  nat numMut = rng.samplePoisson(lambdaSmall);

#ifdef DEBUG_UPDATE_GRAPH
  cout << "numMut(neutral)="  << numMut << " lambda=" << lambda << "\tlambdaSmall="<<  lambdaSmall << "\tpopSize=" << popMan[0].getPopSizeByGeneration(genNum) << "\tgenNum=" << genNum << endl; 
#endif

  for(nat i = 0 ; i < numMut; ++i)
    {
      seqLen_t pos = rng.Integer<nat>(seqLen);
      nat indiNr = rng.Integer<nat>(numSurvivors);
      indiNr = *(start + indiNr); 

      if( regMan.locusSurvivesInPresent(indiNr, pos) && chrom.locusIsNeutral(pos) )
	{
#ifdef DEBUG_UPDATE_GRAPH
	  cout << "neutral mutation, pos=" << pos  << ", indiNr=" << indiNr << ", genNum=" << genNum << endl; 
#endif
	  graph.touchNode(pos, indiNr, genNum, NodeType::MUTATION);
	}
    }
}


void Ancestry::updateGraph_inner(ThreadPool &tp, Survivors &survivors, Chromosome &chromosome, const PopulationManager &popMan, Graph &graph, nat maxPopSize)
{  
  RecombinationManager &recMan = *(recMans[chromosome.getId()]); 
  maxPopSize = MULT_2(maxPopSize); 
  RegionManager regMan(maxPopSize, chromosome.getSeqLen());
  // RecombinationManager &recMan = chromosome.getRecombinationManager();
  
  // EVENTS: this stores the FIRST event for an individual 
  Recombination **events = (Recombination**) calloc( maxPopSize, sizeof(Recombination*)); 

  // backwards iteration through generations
  nat startGen = this->getStartGen();
  for(int curGenIdx = this->getEndGen() - 1 ; curGenIdx >= signed(startGen); --curGenIdx)
    {
      nat currentPopSize = popMan.getTotalNumHaploByGen(curGenIdx);

      survivors.reserve(currentPopSize); 
      
      Recombination *recIterOrig = recMan.getFirstRecByGen(curGenIdx); 
      Recombination *recIterEnd = recMan.getLastRecByGen(curGenIdx); 
      nat numRec = recMan.getNumRecByGen(curGenIdx); 
      
#ifdef DEBUG_UPDATE_GRAPH
      cout << ">>>>>>>>>>>>>>>> GENERATION "  << curGenIdx << "\tnumber of recs "<< numRec << endl; 
#endif
      
      // set up an array for quick look-ups       
      events = (Recombination**)myRealloc(events, currentPopSize * sizeof(Recombination*));
      memset(events, 0, currentPopSize *  sizeof(Recombination*)); 
      for(nat j = 0;  j < numRec; ++j)	
	{
 	  nat num = recIterOrig[j].haploIndiNr; 
	  if(NOT events[num])
	    events[num] = &(recIterOrig[j]); 
	}
      
      // iterate through previous survivors
      nat *end = survivors.getFirstSurvivorForward(curGenIdx-1); 
      for(nat *prevSurvivors = survivors.getFirstSurvivorForward(curGenIdx); 
	  prevSurvivors != end; 
	  ++prevSurvivors)
	{
	  const nat nowIdx = *prevSurvivors;
	  nat ancstIdx = getAddrOfParent(curGenIdx, chromosome.getId(), nowIdx); 

  	  const seqLen_t survivedRegStart = regMan.getRegionStart(nowIdx);
  	  const seqLen_t survivedRegEnd = regMan.getRegionEnd(nowIdx);

#ifdef DEBUG_UPDATE_GRAPH 
	  cout << "* survivor "  << nowIdx << "\t ancestor " <<  ancstIdx << " and ancestrial region " << survivedRegStart << "/" << survivedRegEnd << endl; 
#endif

	  // is this haplotype a recombinant?  
	  if(events[nowIdx])
	    { 
	      assert(nowIdx == events[nowIdx]->haploIndiNr); 
	      nat otherAncst = GET_OTHER_ANCESTOR(ancstIdx); 

	      seqLen_t startSeg = 0; 
	      seqLen_t stopSeg = -1; 
	      for(Recombination* recIter = events[nowIdx] ; 		  
		  recIter < recIterEnd &&
		    recIter->haploIndiNr == nowIdx ; 
		  ++recIter) 
		{
		  startSeg = stopSeg+1; 
		  stopSeg = recIter->absPos-1;

#ifdef DEBUG_UPDATE_GRAPH
		  cout << "\trecombination in " << ancstIdx << "/" << otherAncst << " treating seg "  << startSeg << "-" << stopSeg << endl; 
#endif
		  
		  if( survivedRegStart < stopSeg && startSeg < survivedRegEnd ) // something is transmitted 
		    { 
		      if(stopSeg < survivedRegEnd)	// recombination divides ancestral region 
			{
			  graph.touchNode(stopSeg+1, nowIdx, curGenIdx, RECOMBINATION ); 
#ifdef DEBUG_UPDATE_GRAPH
			  cout  << "\tADDING node for rec " << stopSeg+1 << endl; 
#endif
			}

		      survivors.addSurvivor(ancstIdx);
#ifdef DEBUG_UPDATE_GRAPH
		      cout << "\tmarking region of survivor " << ancstIdx << " with " << max(survivedRegStart, startSeg) << "-"<< min(stopSeg, survivedRegEnd) << endl; 
#endif
		      regMan.extendPrevRegion(ancstIdx, 
					      max(survivedRegStart, startSeg),
					      min(stopSeg, survivedRegEnd)); 
		    }
		  
		  std::swap(ancstIdx, otherAncst);
		}

	      // treat last part 
	      startSeg = stopSeg + 1;
	      stopSeg  = chromosome.getSeqLen(); 

	      if(survivedRegStart < stopSeg && startSeg < survivedRegEnd) 
		{
		  survivors.addSurvivor(ancstIdx); 
#ifdef DEBUG_UPDATE_GRAPH
		  cout << "\tlast part: marking region of survivor "<< ancstIdx << " with " << max(survivedRegStart, startSeg) << "/" << min(stopSeg, survivedRegEnd) << endl; 
#endif
		  regMan.extendPrevRegion(ancstIdx, 
					  max(survivedRegStart, startSeg), 
					  min(survivedRegEnd, stopSeg)); // was ancstlow		  
		}
	    }
	  else
	    {
	      survivors.addSurvivor(ancstIdx); 
	      regMan.extendPrevRegion(ancstIdx,
				      survivedRegStart, 
				      survivedRegEnd); 
#ifdef DEBUG_UPDATE_GRAPH
	      cout << "\tregular: marking "  << survivedRegStart << "-" << survivedRegEnd << " in  " << ancstIdx << endl; 
#endif
	    }
	} // END  SURVIVOR ITER 
      
      insertNeutralMutations(tp, graph, survivors, popMan, chromosome, regMan, curGenIdx); 
      survivors.prepareNextGenBackwards();
      regMan.nextGenBackwards();
    } // END backwards generation iter

  free(events);
}


void Ancestry::updateGraph(ThreadPool &tp, Chromosome &chromosome, Graph &graph, const PopulationManager &popMan, const GenerationCounter &genCnt)
{

  nat firstGen = genCnt.getStartOfSection(), 
    lastGen = genCnt.getEndOfSection(), 
    numGen = lastGen - firstGen; 
  

#ifdef  DEBUG_UPDATE_GRAPH
  cout << "UPDATING GRAPH "  << endl; 
#endif
  
  nat
    chromId = chromosome.getId() ,
    lastPopsize = popMan.getTotalNumHaploByGen(lastGen), 
    maxPopSize = popMan.getMaximumPopSize(0, firstGen, lastGen);

  Randomness &rng = tp[0].getRNG();

  Survivors survivors(lastPopsize, maxPopSize, genCnt.getStartOfSection(), genCnt.getEndOfSection());
  updateGraph_inner(tp, survivors, chromosome, popMan, graph, maxPopSize);
  graph.hookup(survivors, *this, popMan, chromosome, rng, genCnt.getStartOfSection(), genCnt.getEndOfSection());  
}
