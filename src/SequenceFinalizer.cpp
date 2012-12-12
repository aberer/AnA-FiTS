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
 * For simplicity not sorted currently
 */ 
void SequenceFinalizer::annotateFixedMutations(SelectedArray *fixedSelectedMut, BitSetSeq &fixedNeutral, vector<Node*> &bvMeaning)
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
	  SelectedMutation *newMut =  new  SelectedMutation;
	  Node *node = bvMeaning[i]; 	  
	  newMut->absPos = node->loc; 
	  newMut->generation = node->originGen; 
	  newMut->base = node->base; 
	  newMut->fitness = 1.0; 
	  newMut->indiNr = node->indiNr; 
	  fixedMutations.push_back(newMut); 
	}
    }
}


void SequenceFinalizer::annotateRelevantMutations(BitSetSeq &notPresent, BitSetSeq &fixedRawMut, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences)
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
	SelectedMutation *newMut = new SelectedMutation; 
	newMut->absPos = node->loc; 
	newMut->generation = node->originGen; 
	newMut->base = node->base; 
	newMut->fitness = 1.0; 
	newMut->indiNr = node->indiNr; 
	mutationsInSeqs.push_back(newMut);
      }
  
  // sort array 
  sort(mutationsInSeqs.begin(), mutationsInSeqs.end(), 
       [](const SelectedMutation *a, const SelectedMutation* b) -> bool 
       {
	 if(a->absPos != b->absPos)
	   return a->absPos < b->absPos; 
	 else if(a->generation != b->generation)
	   return a->generation < b->generation; 
	 else if(a->base != b->base)
	   return a->base < b->base; 
	 else  
	   {
	     // :TRICKY: very strange ... occurs with -T g 500 1e-3
	     return a->indiNr < b->indiNr ;  
	   }
       } ); 
}



void SequenceFinalizer::createMergedBitvectors(BitSetSeq &notPresent, BitSetSeq &fixedRawMut, vector<BitSetSeq*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences)
{
  assert(rawNeutralSequences.size() == selectedSequences.size()); 
  nat numSequences = rawNeutralSequences.size(); 
  
  vector<nat> selIndices(numSequences, 0);
  nat neutralIndex = 0; 
  nat neutralBsLen = bvMeaning.size(); 

  for(nat i = 0; i < numSequences; ++i)
    finalSequences.push_back(new BitSetSeq(mutationsInSeqs.size())); 
  
  nat bvIdx = 0 ;   
  for(auto iter : mutationsInSeqs ) 
    {
      // cout << "mut " << bvIdx << " / " <<  mutationsInSeqs.size() << endl;  
      // update neutral index 
      while(neutralIndex < neutralBsLen && ( fixedRawMut.test(neutralIndex) || notPresent.test(neutralIndex)))
	++neutralIndex;
      
      assert(neutralBsLen <= neutralIndex  || mutationsInSeqs[bvIdx]->absPos <=  bvMeaning[neutralIndex]->loc );

      if(neutralIndex < neutralBsLen && *(bvMeaning[neutralIndex]) == *iter)
	{
	  for(nat i = 0; i < numSequences; ++i)
	    if(rawNeutralSequences[i]->test(neutralIndex))
	      finalSequences[i]->set(bvIdx); 
	  neutralIndex++; 
	}
      else 			// a selected mutation 
	{
	  for(nat i = 0; i < numSequences; ++i)
	    {	      
	      if( selIndices[i] < selectedSequences[i]->size()
		  && *iter == *(selectedSequences[i]->at(selIndices[i]))) 
		{
		  finalSequences[i]->set(bvIdx); 
		  selIndices[i]++; 
		}
	    }
	}
      
      bvIdx++; 
    }  
}


void SequenceFinalizer::computeFinalSequences(vector<BitSetSeq*> rawNeutralSequences, vector<Node*> bvMeaning, vector<SelectedArray*> selectedSequences, SelectedArray *fixedSelected)
{  
  // determine mutations not occuring at all 
  BitSetSeq mutationsNotPresent(bvMeaning.size()); 
  for(BitSetSeq *bv : rawNeutralSequences)
    mutationsNotPresent.orify(*bv);
  mutationsNotPresent.flip();    

  // determine fixed neutral mutations   
  BitSetSeq fixedRawMut(bvMeaning.size()); 
  fixedRawMut.set();
  assert(fixedRawMut.count() == bvMeaning.size()); 
  
  for(BitSetSeq *bv : rawNeutralSequences)
    fixedRawMut.andify(*bv);

#ifndef NDEBUG
  BitSetSeq tmp(bvMeaning.size()); 
  tmp.set();
  tmp.andify(fixedRawMut); 
  tmp.andify(mutationsNotPresent); 
  assert(tmp.count() == 0);
#endif
  
  annotateFixedMutations(fixedSelected, fixedRawMut, bvMeaning);  
  annotateRelevantMutations(mutationsNotPresent, fixedRawMut, bvMeaning, selectedSequences); 
  createMergedBitvectors(mutationsNotPresent, fixedRawMut,  rawNeutralSequences, bvMeaning, selectedSequences);   

#ifndef NDEBUG
  BitSetSeq everywhereTmp(finalSequences[0]->size()); 
  everywhereTmp.set();
  for(nat i = 0;  i < finalSequences.size(); ++i)
    everywhereTmp.andify(*(finalSequences[i]));
  assert(everywhereTmp.count() == 0); 

  BitSetSeq noWhereTmp(finalSequences[0]->size()); 
  noWhereTmp.set();
  for(nat i =0; i < finalSequences.size(); ++i)
    noWhereTmp.orify(*(finalSequences[i])); 
  noWhereTmp.flip();
  assert(noWhereTmp.count() == 0);   
#endif
}




void SequenceFinalizer::printBinary(FILE *fh)
{
  // write fixed 
  nat num = fixedMutations.size(); 
  BIN_WRITE(num, fh);  
  for(auto mut :  fixedMutations)
    mut->printRaw(fh);
  
  // write mutations 
  num = mutationsInSeqs.size();
  BIN_WRITE(num, fh); 
  for(auto mut : mutationsInSeqs)
    mut->printRaw(fh);
  
  num = finalSequences.size();
  BIN_WRITE(num,fh);   
  nat ctr = 0; 
  for(auto bv : finalSequences)
    {
#ifdef PRINT_SEQ_STDOUT
      cout << *bv << endl; 
#endif
      bv->printRaw(fh);   
    }
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



