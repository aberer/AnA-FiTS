#pragma once 

#include "types.hpp"
#include "common.hpp"


struct FitnessBucket
{
  FITNESS_TYPE fitness[3]; 
  seqLen_t absPos;
  FitnessBucket *next;
  FITNESS_TYPE& operator[](nat num){return fitness[num]; }
}; 


class HashTable
{
public:
  explicit HashTable(nat numSites); 
  ~HashTable();

  FitnessBucket& operator[](seqLen_t pos);    
  nat getBucketsUsed(){return bucketsUsed; }

private: 
  FitnessBucket **array;
  FitnessBucket *buckets;
  nat bucketsUsed; 
  nat moduloFactor;
 
  bool find(FitnessBucket*& bkt, seqLen_t hash); 
  void insert(FitnessBucket *bkt); 
  seqLen_t hashFunction(seqLen_t pos);
};


seqLen_t findNextPower2(seqLen_t num); 


////////////////////
// IMPLEMENTATION //
////////////////////
inline seqLen_t  HashTable::hashFunction(seqLen_t pos)
{
  return pos & moduloFactor; 
}




/**  
     wraps find and provides a new element, if stuff has not yet been
     inserted into hashtable 
 */
inline FitnessBucket& HashTable::operator[](seqLen_t pos)
{
  FitnessBucket *bkt = nullptr; 

  if(find(bkt, pos))
    return *bkt; 
  else 
    {
      bkt = buckets + bucketsUsed; 
      bucketsUsed++; 
      bkt->absPos = pos; 
      insert(bkt);
      return *bkt; 
    }
}


inline bool HashTable::find(FitnessBucket*& bkt, seqLen_t pos )
{  
  bkt = array[hashFunction(pos)]; 
  
  while(bkt != nullptr && bkt->absPos != pos)
    bkt = bkt->next; 
  
  return bkt != nullptr  && bkt->absPos == pos; 
}


inline void HashTable::insert(FitnessBucket *bkt) 
{
  seqLen_t hash = hashFunction(bkt->absPos);
  bkt->next = array[hash]; 
  array[hash] = bkt;   
}
