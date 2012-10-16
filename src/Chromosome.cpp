#include "Chromosome.hpp" 

#include <vector>


Chromosome::Chromosome(seqLen_t seqLen, bool _isNeutral, FitnessFunction fFun, nat _id, nat numPop)
  : seqLen(seqLen)
  , uniqueHaplotypes(1000) // :MAGIC:
  , seqRep(seqLen)
  , mutMan(seqLen,fFun)
  , id(_id)
  , isNeutral(_isNeutral)
{
  for(nat i = 0; i < numPop; ++i)
    fixedMutations.push_back(new SelectedArray(100)); 
}


Chromosome::~Chromosome()
{
  for(auto seq : fixedMutations)
    delete seq; 
}


void Chromosome::init(Randomness &rng)
{
  seqRep.initSequence(rng);  
}


void Chromosome::updateUniqueHaplotypes(SelectedArray** seqBegin,SelectedArray** seqEnd)
{
  uniqueHaplotypes.resetUsed();
  for(auto seqI  = seqBegin; seqI != seqEnd; ++seqI)
    {
      if(NOT (*seqI)->wasVisited())
	{
	  (*seqI)->visit();
	  uniqueHaplotypes.setNext(*seqI) ; 
	}
    } 
}


#define CURRENT_HAS_MUTATION (currentIndex[i] < seq.size())
#define CURRENT_IS_GREATER (seq.at(currentIndex[i])->absPos < currentMut->absPos  )
#define CURRENT_IS_LESS ( currentMut->absPos < seq.at(currentIndex[i])->absPos   )

// :BUG: 
void Chromosome::cleanFixedMutations(SelectedArray** seqBegin,SelectedArray** seqEnd, nat popId, ThreadPool &tp)
{
#ifdef DEBUG_ALT_CLEANUP
  cout << "cleaning up chromo " <<  id << endl; 
#endif

  updateUniqueHaplotypes(seqBegin, seqEnd);  

#ifdef DEBUG_ALT_CLEANUP
  cout << "Total of  "<<   uniqueHaplotypes.getUsed() << endl; 
#endif

  // search indices that are present in each state 
  vector<nat> currentIndex(uniqueHaplotypes.getUsed(),0); 
  assert(currentIndex[0] == 0); 

  bool foundNewOne = true;   
  SelectedMutation *currentMut = nullptr;   
  while(foundNewOne)
    {
      foundNewOne = false;
      nat i = 0; 
      SelectedMutation *init = currentMut; 
      while(i < uniqueHaplotypes.getUsed())
	{
	  SelectedArray &seq =  *(uniqueHaplotypes.at(i));

	  if(CURRENT_HAS_MUTATION)
	    {
	      if(currentMut == nullptr)
		{
		  currentMut = seq.at(currentIndex[i]); 
		  foundNewOne = true; 
		  break; 
		}
	      else if( CURRENT_IS_LESS )
		{ 
		  currentMut = seq.at(currentIndex[i]) ;		  
		  foundNewOne = true; 
		  break; 
		}
	      else if( CURRENT_IS_GREATER)
		{		  
		  while(CURRENT_HAS_MUTATION && CURRENT_IS_GREATER)		    
		    currentIndex[i]++;
		  if (CURRENT_HAS_MUTATION  && CURRENT_IS_LESS)
		    {
		      currentMut = seq.at(currentIndex[i]); 
		      foundNewOne = true;
		      break;
		    }
		  else if(NOT CURRENT_HAS_MUTATION)
		    {
		      currentMut = nullptr; 
		      foundNewOne = false; 
		      break;
		    }
		}
	    }
	  else
	    {
	      currentMut = nullptr; 
	      foundNewOne  = false; 
	      break;
	    }

	  i++;
	}
      
      if(currentMut && init == currentMut)
	{
	  // assert that everybody has it 
	  for(nat j = 0; j < uniqueHaplotypes.getUsed(); ++j)
	    assert(uniqueHaplotypes.at(j)->getIfPresent(currentMut->absPos) != nullptr); 

#ifdef DEBUG_REPORT_FIXATIONS
	  cout << *currentMut << " went to fixation ; removing " << endl; 
#endif

	  for(nat j = 0; j < uniqueHaplotypes.getUsed();++j)
	    {
	      assert(uniqueHaplotypes.at(j)->getIfPresent(currentMut->absPos)->absPos == currentMut->absPos); 
	      uniqueHaplotypes.at(j)->removeAt(currentIndex[j]) ;
	    }
	  
	  for(nat j = 0; j < uniqueHaplotypes.getUsed(); ++j)
	    assert(uniqueHaplotypes.at(j)->getIfPresent(currentMut->absPos) == nullptr); 

	  insertFixedMutation(currentMut, popId);

	  foundNewOne = true; 
	  currentMut = nullptr; 
	}
    }

  for(SelectedArray *seqI : uniqueHaplotypes)
    {
      seqI->computeFitness(); 
      seqI->resetVisited();
    }  

  // unclaim mutations
  for(nat i = 0; i < tp.getNumberOfThreads() ;++i)
    {
      ResizMemArena<SelectedMutation> &mem = tp[i].getMutationMemory(); 
      mem.unclaimAll();
    }

  // reclaim mutations
  for(SelectedArray* seqI : uniqueHaplotypes)
    seqI->claimMutations();
}


void Chromosome::insertFixedMutation(SelectedMutation *mut, nat popId)
{
  SelectedArray *array = fixedMutations[popId];  
  array->mutate(*mut, false, true); //  :TODO: correct? was true,nothing before   
}

