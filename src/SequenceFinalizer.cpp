#include "SequenceFinalizer.hpp"
#include <algorithm>


SequenceFinalizer::SequenceFinalizer()
  : finalSequences(0)
  , mutationsInSeqs(0)
  , fixedMutations(0)
{
}



// :TRICKY: we copy everything, so we have to destroy it as well 
SequenceFinalizer::~SequenceFinalizer()
{
  for(nat i = 0; i < finalSequences.size(); ++i)
    delete finalSequences[i]; 
  for(nat i = 0; i < mutationsInSeqs.size(); ++i)
    delete mutationsInSeqs[i]; 
  for(nat i = 0; i < fixedMutations.size(); ++i)
    delete fixedMutations[i]; 
}



/** 
    For simplicity not sorted currently
 */ 
void SequenceFinalizer::annotateFixedMutations(SelectedArray *fixedSelectedMut, BitSet<uint64_t> &fixedNeutral, vector<Node*> &bvMeaning)
{  
  for(SelectedMutation** iter = fixedSelectedMut->begin(); iter != fixedSelectedMut->end(); ++iter)
    {
      SelectedMutation *newMut = new SelectedMutation(**iter);
      fixedMutations.push_back(newMut); 
    }

  for(nat i = 0; i < fixedNeutral.size(); ++i)
    {
      if(fixedNeutral.test(i))
	{
	  SelectedMutation *newMut = (SelectedMutation*)calloc(1,sizeof(SelectedMutation));
	  Node *node = bvMeaning[i]; 	  
	  newMut->absPos = node->loc; 
	  newMut->generation = node->originGen; 
	  newMut->base = node->base; 
	  newMut->fitness = 0; 
	  newMut->indiNr = node->indiNr; 
	  fixedMutations.push_back(newMut); 
	}
    }
}


void SequenceFinalizer::annotateRelevantMutations(BitSet<uint64_t> &notPresent, BitSet<uint64_t> &fixedRawMut, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences)
{
  // insert non-neutral mutations
  for(SelectedArray *seq : selectedSequences)
    for(auto iter = seq->begin(); iter != seq->end(); ++iter)
      (*iter)->unclaim();
  
  for(SelectedArray *seq : selectedSequences)
    for(auto iter = seq->begin(); iter != seq->end(); ++iter)
      {	
	if(NOT (*iter)->isClaimed())
	  {	    
	    (*iter)->claim();
	    auto tmp = new SelectedMutation(**iter); 
	    mutationsInSeqs.push_back(tmp); 
	  }
      }

  // insert neutral mutations 
  for(nat i = 0; i < bvMeaning.size(); ++i)
    if(NOT notPresent.test(i) && NOT fixedRawMut.test(i))
      {
	Node *node = bvMeaning[i]; 
	SelectedMutation *newMut = (SelectedMutation*) calloc(1,sizeof(SelectedMutation)); 
	newMut->absPos = node->loc; 
	newMut->generation = node->originGen; 
	newMut->base = node->base; 
	newMut->fitness = 0; 
	newMut->indiNr = node->indiNr; 
	mutationsInSeqs.push_back(newMut);
      }
  
  // sort array 
  sort(mutationsInSeqs.begin(), mutationsInSeqs.end(), [](const SelectedMutation *a, const SelectedMutation* b) -> bool {return a->absPos < b->absPos ; }); 
}


void SequenceFinalizer::createMergedBitvectors(BitSet<uint64_t> &notPresent, BitSet<uint64_t> &fixedRawMut, vector<BitSet<uint64_t>*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences)
{
  assert(rawNeutralSequences.size() == selectedSequences.size()); 
  nat numSequences = rawNeutralSequences.size(); 
  
  vector<nat> selIndices(numSequences, 0);
  nat neutralIndex = 0; 
  nat neutralBsLen = bvMeaning.size(); 

  for(nat i = 0; i < numSequences; ++i)
    finalSequences.push_back(new BitSet<uint64_t>(mutationsInSeqs.size())); 
  
  nat bvIdx = 0 ; 
  for(auto iter : mutationsInSeqs ) 
    {
      // update neutral index 
      while(neutralIndex < neutralBsLen && ( fixedRawMut.test(neutralIndex) || notPresent.test(neutralIndex)))
	++neutralIndex;
      
      if(neutralIndex < neutralBsLen && *(bvMeaning[neutralIndex]) == *iter)
	{
	  for(nat i = 0; i < numSequences; ++i)
	    if(rawNeutralSequences[i]->test(neutralIndex))
	      finalSequences[i]->set(bvIdx); 
	  neutralIndex++; 
	}
      else 			// a selected mutation 
	{
	  bool foundOne = false; 
	  for(nat i = 0; i < numSequences; ++i)
	    {	      
	      if(*iter == *(selectedSequences[i]->at(selIndices[i]))) 
		{
		  finalSequences[i]->set(bvIdx); 
		  selIndices[i]++; 
		  foundOne = true; 
		}
	    }
	  assert(foundOne); 
	} 
      
      bvIdx++; 
    }
  
}


void SequenceFinalizer::computeFinalSequences(vector<BitSet<uint64_t>*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences, SelectedArray *fixedSelected)
{  
  // determine mutations not occuring at all 
  BitSet<uint64_t> mutationsNotPresent(bvMeaning.size()); 
  for(BitSet<uint64_t> *bv : rawNeutralSequences)
    mutationsNotPresent.orify(*bv);
  mutationsNotPresent.flip();    

  // determine fixed neutral mutations   
  BitSet<uint64_t> fixedRawMut(bvMeaning.size()); 
  fixedRawMut.set();
  assert(fixedRawMut.count() == bvMeaning.size()); 
  
  for(BitSet<uint64_t> *bv : rawNeutralSequences)
    fixedRawMut.andify(*bv);

#ifndef NDEBUG
  BitSet<uint64_t> tmp(bvMeaning.size()); 
  tmp.set();
  tmp.andify(fixedRawMut); 
  tmp.andify(mutationsNotPresent); 
  assert(tmp.count() == 0);
#endif

  annotateFixedMutations(fixedSelected, fixedRawMut, bvMeaning);
  annotateRelevantMutations(mutationsNotPresent, fixedRawMut, bvMeaning, selectedSequences); 
  createMergedBitvectors(mutationsNotPresent, fixedRawMut,  rawNeutralSequences, bvMeaning, selectedSequences); 
}




bool operator==(SelectedMutation &lhs, SelectedMutation &rhs)
{
  return 
    lhs.absPos == rhs.absPos 
    && lhs.generation == rhs.generation 
    && lhs.base == rhs.base 
    && lhs.indiNr == rhs.indiNr ; 
}  

bool operator==(SelectedMutation &lhs, Node &rhs)
{
  return 
    lhs.absPos == rhs.loc 
    && lhs.generation == rhs.originGen
    && lhs.base == rhs.base 
    && lhs.indiNr == rhs.indiNr; 
}
  
bool operator==(Node &lhs, SelectedMutation &rhs)
{
  return rhs == lhs; 
}
