#include "Chromosome.hpp" 

#include <vector>


Chromosome::Chromosome(seqLen_t seqLen, FitnessFunction fFun, nat _id)
  : seqLen(seqLen)
  , uniqueHaplotypes(1000) // :MAGIC:
  , seqRep(seqLen)
  , mutMan(seqLen,fFun)
  // , recMan(unique_ptr<RecombinationManager>(new RecombinationManager(seqLen)))
  , id(_id)
{
  
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


#define OLD 

#ifdef OLD 
void Chromosome::cleanFixedMutations(SelectedArray** seqBegin,SelectedArray** seqEnd, nat popId, ThreadPool &tp)
{
  updateUniqueHaplotypes(seqBegin, seqEnd);  

#ifdef DEBUG_ALT_CLEANUP
  cout << "entering reorganization. Total of  "<<   uniqueHaplotypes.getUsed() << endl; 
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
#ifdef DEBUG_ALT_CLEANUP
	  cout  << "entering sequence " << (&seq) << ":"  <<  seq << endl; ; 
#endif

	  if(CURRENT_HAS_MUTATION)
	    {
#ifdef DEBUG_ALT_CLEANUP
	      cout << "\thas something "<< endl; 
#endif
	      if(currentMut == nullptr)
		{
		  currentMut = seq.at(currentIndex[i]); 
#ifdef DEBUG_ALT_CLEANUP
		  cout << "\tcurrentMut is nullptr; new one is "<<  currentMut->absPos << endl; 
#endif
		  foundNewOne = true; 
		  break; 
		}
	      else if( CURRENT_IS_LESS )
		{		  
#ifdef DEBUG_ALT_CLEANUP
		  cout << "\tcurrentMut is " << currentMut->absPos; 
#endif
		  currentMut = seq.at(currentIndex[i]) ;		  
#ifdef DEBUG_ALT_CLEANUP
		  cout << " replacing with " << currentMut->absPos << endl; 
#endif
		  foundNewOne = true; 
		  break; 
		}
	      else if( CURRENT_IS_GREATER)
		{
#ifdef DEBUG_ALT_CLEANUP
		  cout << "\tcurrentMut is " << currentMut->absPos ; 
#endif
		  
		  while(CURRENT_HAS_MUTATION && CURRENT_IS_GREATER)		    
		    currentIndex[i]++;
#ifdef DEBUG_ALT_CLEANUP
		  cout << " \tmoved current index to  " << currentIndex[i] << endl;  
#endif

		  if (CURRENT_HAS_MUTATION  && CURRENT_IS_LESS)
		    {
		      currentMut = seq.at(currentIndex[i]); 
#ifdef DEBUG_ALT_CLEANUP
		      cout << "\tfound new higher mut " << currentMut->absPos << endl;
#endif
		      foundNewOne = true;
		      break;
		    }
		  else if(NOT CURRENT_HAS_MUTATION)
		    {
#ifdef DEBUG_ALT_CLEANUP
		      cout << "\tnothing there any more after spooling " << endl; 
#endif
		      currentMut = nullptr; 
		      foundNewOne = false; 
		      break;
		    }
		}
	    }
	  else
	    {
#ifdef DEBUG_ALT_CLEANUP
	      cout << "\tthat's it!" << endl; 
#endif
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

#ifdef DEBUG_ALT_CLEANUP
	  cout << "TRYING TO REMOVE "<< currentMut->absPos << endl; 
#endif

#ifdef DEBUG_REPORT_FIXATIONS
	  cout << *currentMut << "went to fixation ; removing " << endl; 
#endif

	  for(nat j = 0; j < uniqueHaplotypes.getUsed();++j)
	    {
#ifdef DEBUG_ALT_CLEANUP
	      cout <<  "in "  <<  uniqueHaplotypes.at(j) << *(uniqueHaplotypes.at(j)) << endl; 	  
#endif
	      assert(uniqueHaplotypes.at(j)->getIfPresent(currentMut->absPos)->absPos == currentMut->absPos); 
	      uniqueHaplotypes.at(j)->removeAt(currentIndex[j]) ;
	    }
	  
	  for(nat j = 0; j < uniqueHaplotypes.getUsed(); ++j)
	    assert(uniqueHaplotypes.at(j)->getIfPresent(currentMut->absPos) == nullptr); 

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
#else 

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


#endif


void Chromosome::insertFixedMutation(SelectedMutation *mut, nat popId)
{
  while(fixedMutations.size() < popId+1)
    fixedMutations.push_back(new SelectedArray(100)); 

  SelectedArray *array = fixedMutations[popId];  
  array->mutate(*mut, true);
}

