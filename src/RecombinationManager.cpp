#include "RecombinationManager.hpp"

#include "common.hpp"
#include "ThreadPool.hpp"
#include "Randomness.hpp"
#include "FractionalSimulation.hpp"

int sortByAbsPos(const void* t1, const void* t2) 
{ 
  return ((Recombination*)t1)->absPos - ((Recombination*)t2)->absPos; 
}



RecombinationManager::RecombinationManager(seqLen_t _seqLen, nat _startGen, nat _endGen)
  : TimeSection(_startGen, _endGen)
  , totalRec(0)
  , recombs(nullptr)
  , recIndex(0)
  , firstRecPerGen(0)
  , numRecPerGen(0)
  , seqLen(_seqLen)
{ 
}


RecombinationManager::~RecombinationManager()
{
  free(recombs); 
}


void RecombinationManager::determineRecsPerGen(ThreadPool &tp, const PopulationManager &popMan)
{
  Randomness &rng = tp[0].getRNG();  
  nat endGen = this->getEndGen();
  for(nat i = this->getStartGen();  i < endGen ; ++i )
    { 
      float lambda = PopulationManager::getLamdbaForParam(popMan[0].getRecombinationRate(i), seqLen, popMan, 0, i); 
      nat recsHere = rng.samplePoisson(lambda ); 
      numRecPerGen.at(getGenIdx(i)) = recsHere;
      totalRec += recsHere;
    }
}


ostream& operator<<(ostream &stream, const RecombinationManager &rhs)
{
  nat endGen = rhs.getEndGen();
  for(nat i = rhs.getStartGen(); i < endGen; ++i)
    {
      stream << "recs in gen " << i << endl; 
      for(nat j = 0; j < rhs.numRecPerGen[i]; ++j)
	{
	  Recombination *rec = rhs.firstRecPerGen[i] + j ; 
	  // cout << "[" << rec->absPos << "," << rec->haploIndiNr << "],"  ; 
	}
      stream << endl;  
    }
  return stream; 
}


void RecombinationManager::precompute(ThreadPool &tp, const PopulationManager &popMan)
{
  numRecPerGen = vector<nat>(this->getNumGen()); 
  firstRecPerGen  = vector<Recombination*>(this->getNumGen()); 

  Randomness &rng = tp[0].getRNG(); 

  // :KLUDGE:
  nat popSize = popMan.getTotalNumHaploByGen(0); 
  nat *numRecPerIndi = (nat*)calloc(popSize, sizeof(nat));  

  determineRecsPerGen(tp, popMan);
  recombs = (Recombination*)calloc(totalRec, sizeof(Recombination)); 

  auto recC = 0U; 
  for(nat genC = getStartGen(); genC < getEndGen() ; ++genC)
    {
      auto numRec = numRecPerGen[this->getGenIdx(genC)]; 
      firstRecPerGen[this->getGenIdx(genC)] = (numRec > 0 ) ? &(recombs[recC]) : nullptr; 

      if( numRec)
	{
	  // compute a map: how many recs per individual in generation 
	  nat popSizeL = popMan.getTotalNumHaploByGen(genC);	  
	  // cout << "in gen " << genC << " size is "  << popSizeL << endl; 
	  size_t byteSize =  sizeof(nat) * popSizeL; 

	  numRecPerIndi = (nat*)myRealloc(numRecPerIndi, byteSize) ; 
	  memset(numRecPerIndi, 0, byteSize); 
	  
	  // do the acutal sampling 
	  CHOOSE_IMPLEMENTATION_BY_TYPE(popSizeL, precompute_helper, tp, numRecPerIndi, numRec, popSizeL); 

	  // compute recombinations
	  for(nat i = 0; i < popSizeL ; ++i) 
	    { 
	      nat numRecHere = numRecPerIndi[i];
	      for(nat numRecIndi = 0 ; numRecIndi <  numRecHere ; ++numRecIndi)
		{
		  Recombination& rec = recombs[recC++]; 
		  assert(recC <= totalRec); 
		  
		  rec.absPos = rng.Integer(seqLen); 
		  rec.haploIndiNr = i;

		  // cout << "\t" << rec.haploIndiNr << endl; 
		}
	    }
	}
    } 
  free(numRecPerIndi); 
}


