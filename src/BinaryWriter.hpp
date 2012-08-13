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

  template<class TYPE> void getSortedMutations(vector<TYPE*> &result, const vector<SequenceArray<TYPE>*> &arrays); 
  void convertToBitSet(BitSet<uint32_t> &bs, NeutralArray *nSeq, SelectedArray *sSeq,  const vector<NeutralMutation*> &neutralMutations, const vector<SelectedMutation*> &selMutations); 
  ostream& printMutations(ostream &rhs , FILE *fh, vector<NeutralMutation*> &neutralMutations, vector<SelectedMutation*> &selMutations); 
}; 


////////////////////
// IMPLEMENTATION //
////////////////////
template<class TYPE> void BinaryWriter::getSortedMutations(vector<TYPE*> &result, const vector<SequenceArray<TYPE>*> &arrays)
{
  set<TYPE*> mutationMap;

  for(auto array : arrays)
    if(array)
      for(TYPE **mutIter = array->begin(); mutIter != array->end(); ++mutIter)
	mutationMap.insert(*mutIter); 
  
  for(auto iter = mutationMap.begin(); iter != mutationMap.end(); ++iter)
    result.push_back(*iter);   
  sort(result.begin(), result.end(), [](TYPE *a, TYPE *b){return a->absPos < b->absPos ; }); 
}



