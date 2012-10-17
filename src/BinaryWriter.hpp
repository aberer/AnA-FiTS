#pragma once 

#include "Graph.hpp"
#include "HaploTimeWindow.hpp"

#include <set>
#include <iostream>
#include <string>

class BinaryWriter
{
public:
  BinaryWriter(string id, nat numHaplo); 
  ~BinaryWriter();
  void write(Graph &graph, HaploTimeWindow &w, Chromosome &chrom);
  void writeInt(nat theInt); 

private: 
  FILE *fh; 
  nat numHaplo; 

#ifndef USE_BVBASED_EXTRACTION
  template<class TYPE> void getSortedMutations(vector<TYPE*> &result, const vector<SequenceArray<TYPE>*> &arrays); 
#endif
  void convertToBitSet(BitSet<uint32_t> &bs, NeutralArray *nSeq, SelectedArray *sSeq,  const vector<NeutralMutation*> &neutralMutations, const vector<SelectedMutation*> &selMutations); 
  ostream& printMutations(ostream &rhs , FILE *fh, vector<NeutralMutation*> &neutralMutations, vector<SelectedMutation*> &selMutations); 
}; 


////////////////////
// IMPLEMENTATION //
////////////////////


#ifndef USE_BVBASED_EXTRACTION
/* 
   get a sorted array of all mutations for conversion of sequences into bitvectors. 
   :TRICKY: this depends on all mutations being unclaimed first
 */ 
template<class TYPE> void BinaryWriter::getSortedMutations(vector<TYPE*> &result, const vector<SequenceArray<TYPE>*> &arrays)
{
  for(auto array : arrays)
    {
      TYPE **mutPtrEnd = array->end(); 
      for(TYPE **mutPtr = array->begin(); mutPtr < mutPtrEnd; ++mutPtr)	
	(*mutPtr)->unclaim();
    }

  SequenceArray<TYPE> tmpSeq(1000); 

  for(auto array : arrays )
    {
      TYPE **mutPtrEnd = array->end(); 
      for(TYPE **mutPtr =  array->begin(); mutPtr < mutPtrEnd; ++mutPtr)
	{
	  if(NOT (*mutPtr)->isClaimed())
	    {
	      (*mutPtr)->claim();
	      tmpSeq.mutate(**mutPtr, false, true);
	    }
	}
    }

  TYPE** mutPtrEnd = tmpSeq.end();
  for(TYPE** mutPtr = tmpSeq.begin(); mutPtr < mutPtrEnd; ++mutPtr)
    result.push_back(*mutPtr);   
}
#endif
