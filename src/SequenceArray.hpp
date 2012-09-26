#pragma once 

#include "common.hpp"
#include "Recombination.hpp"
#include "types.hpp"
#include "Mutation.hpp"

#include <cassert>
#include <cstring>
#include <iostream>

template<class MUTATION>
class SequenceArray
{
public: 
  // LIFE CYCLE 
  explicit SequenceArray(nat initSize); 
  ~SequenceArray(); 
  SequenceArray& operator=(const SequenceArray& rhs); 

  // OPERATORS
  void mutate(MUTATION &mut, bool replace);  
  void recombine(const SequenceArray &seqA, const SequenceArray &seqB, Recombination *start, nat length); 
  void remove(MUTATION *mut);
  void removeAt(nat index); 
  nat searchIndexSmallerThan(seqLen_t key) const;  
  void erase();
  void copyRegion(const SequenceArray *donor, nat start, nat end); 
  MUTATION* getIfPresent(seqLen_t key ) const;   
  void computeFitness(); 
  void resetVisited(){ wasVisited_v = false; }
  void claimMutations();
  void conditionallyJoinOtherSeq(const SequenceArray &rhs, seqLen_t start, seqLen_t end); 

  // OBSERVERS
  nat size() const {return numElem;} 
  void printSeq(FILE *fp) const; 

  // ITERATORS
  MUTATION** begin() const {return array; } 
  MUTATION** end() const {return array + numElem;}

  // ACCESSORS 
  MUTATION*& operator[](nat num);  
  MUTATION* at(nat num) const {return array[num];}  
  void visit(){wasVisited_v = true; }
  bool wasVisited(){return wasVisited_v; }    
  FITNESS_TYPE getFitness(){return fitness;}
  nat getGeneration() {return usedInGeneration; }
  void setGeneration(nat gen) {usedInGeneration = gen; }
  nat getCreationGen(){return creationGen ; }
  void setCreationGen(nat cGen){creationGen = cGen; }
  void setFitness(FITNESS_TYPE fit){fitness = fit; }
  
private: 
  MUTATION **array; 
  nat numElem; 
  nat capacity; 
  bool wasVisited_v;   
  FITNESS_TYPE fitness; 
  nat usedInGeneration;
  nat creationGen; 

  SequenceArray(const SequenceArray &rhs); 
  void resize();
  void checkConsistency(); 
  void mutate_helper(MUTATION &mut, bool replace); 
  
  template<class MUT>
  friend ostream& operator<<(ostream &stream, const SequenceArray<MUT> &rhs);    
};


typedef SequenceArray<SelectedMutation> SelectedArray; 
typedef SequenceArray<NeutralMutation> NeutralArray; 


////////////////////
// IMPLEMENTATION //
////////////////////


template<class MUTATION>
inline MUTATION*& SequenceArray<MUTATION>::operator[](nat num)
{
  assert(num < numElem); 
  return array[num]; 
}


template<class MUTATION>
inline void SequenceArray<MUTATION>::removeAt(nat index)
{
  assert(numElem);
  if(numElem == 1)
    {
      numElem--;
      return;
    }

  memmove(array + index, array + index + 1, (numElem - index - 1) * sizeof(MUTATION*)); 
  numElem--; 
}


// invariants: key > A[i] for all i < low
// key <= A[i] for all i > high
template<class MUTATION>
inline nat SequenceArray<MUTATION>::searchIndexSmallerThan(seqLen_t key)  const
{
  int low = 0; 
  int high = numElem - 1;
  while (low <= high) 
    {
      nat mid = DIVIDE_2((low + high)); 
      if (key <= array[mid]->absPos)
	high = mid - 1 ;
      else
	low = mid + 1; 
    }
  return low; 
}




template<class MUTATION>
SequenceArray<MUTATION>::SequenceArray(nat initSize)
  : numElem(0)
  , capacity(initSize)
  , wasVisited_v(false)
  , fitness(1.f)
  , usedInGeneration(-3)		// :TRICKY: ensures that we can compare generations
  , creationGen(-3)
{
  array = (MUTATION**)calloc(capacity, sizeof(MUTATION*)); 
}

template<class MUTATION>
SequenceArray<MUTATION>::~SequenceArray()
{
  free(array); 
}


template<class MUTATION>
void SequenceArray<MUTATION>::mutate_helper(MUTATION &mut, bool replace)
{
  if(capacity == numElem )
    resize();
  
  nat index = searchIndexSmallerThan(mut.absPos);   

  if(index == numElem)
    {
      array[numElem] = &mut; 	// append to end 
      numElem++;
    }
  else if( array[index]->absPos == mut.absPos )
    {
      if(replace)
	array[index] = &mut; 	// replace current mutation    
    }
  else				
    {				// insert ; expensive memmove  
      nat length = numElem - index;
      assert(length); 
      memmove(array + index + 1, array + index, length * sizeof(MUTATION*)); 
      array[index] = &mut; 
      numElem++;
    }

#ifndef NDEBUG
  checkConsistency();
#endif  
}



// :TRICKY: assuming it IS there 
template<class MUTATION>
void SequenceArray<MUTATION>::remove(MUTATION *mut)
{
  if(numElem == 1)
    {
      numElem--; 
      return; 
    }
  nat index = searchIndexSmallerThan(mut->absPos); 
  memmove( array + index , array + index + 1, (numElem - index - 1) * sizeof(MUTATION*) ); 
  numElem--; 
}


template<class MUTATION>
void SequenceArray<MUTATION>::recombine(const SequenceArray &seqA, const SequenceArray &seqB, Recombination *start, nat length)
{
  assert(this != &seqA); 
  assert(this != &seqB); 
  erase();
  
  const SequenceArray *primaryDonor = &seqA, 
    *otherDonor = &seqB; 
  
  nat i = 0; 
  while(i <= length)
    {
      nat startAbs = (i == 0) ? 0 : start[i-1].absPos; 
      nat endAbs = (i == length)? -1 : start[i].absPos;
      
      copyRegion(primaryDonor,startAbs, endAbs); 
      swap(primaryDonor, otherDonor); 
      ++i;
    }

#ifndef NDEBUG
  checkConsistency();
#endif
}


template<class MUTATION>
void SequenceArray<MUTATION>::erase()
{
  numElem = 0; 
  fitness = 1.0; 
}


template<class MUTATION>
SequenceArray<MUTATION>& SequenceArray<MUTATION>::operator=(const SequenceArray& rhs)
{
  if(this == &rhs)
    return *this; 
  
  if(NOT rhs.size())
    {
      this->numElem = 0; 
      return *this; 
    }

  while(capacity < rhs.numElem)
    resize();

  this->fitness = rhs.fitness;
  this->numElem = rhs.numElem; 
  this->wasVisited_v = false; 
  this->usedInGeneration = rhs.usedInGeneration; 
  this->creationGen = rhs.creationGen; 
  if(numElem) 
    memcpy(array, rhs.array,numElem * sizeof(MUTATION*));
  
  return *this; 
}


template<class MUTATION>
void SequenceArray<MUTATION>::resize()
{
  if(NOT capacity )
    capacity = 100; 
  else
    capacity *= 2;   

  array = (MUTATION**)myRealloc(array, capacity * sizeof(MUTATION*)); 
}


template<class MUTATION>
void SequenceArray<MUTATION>::checkConsistency()
{
#ifdef DEBUG_CHECK_ARRAY_CONSISTENCY
  // cout << "checking consistency " << endl; 
  for(nat i = 0; i < numElem; ++i)
    assert(array[i]); 
#endif
}


template<class MUTATION>
MUTATION* SequenceArray<MUTATION>::getIfPresent(seqLen_t key ) const
{
  if(numElem == 0)
    return nullptr; 

  int imin = 0; 
  int imax = numElem-1; 
  
  while (imax >= imin)
    {
      int imid = (imin + imax) / 2;
      if (array[imid]->absPos <  key)
        imin = imid + 1;
      else if (array[imid]->absPos > key )
        imax = imid - 1;
      else
        return array[imid];
  }

  return nullptr; 
}


template<class MUTATION>
void SequenceArray<MUTATION>::computeFitness()
{  
  FITNESS_TYPE result = 1.0; 

  for(nat i = 0; i < numElem; ++i)
    result *= array[i]->fitness; 

  this->fitness = result; 
}


// :TRICKY: we are assuming, that the copy is always appended to the end
// :CONVENT: INCLUDING start and INCLUDING END
template<class MUTATION>
void SequenceArray<MUTATION>::copyRegion(const SequenceArray *donor, nat start, nat end)
{
  if(NOT donor)
    return;

  assert(donor != this);
  nat index = donor->searchIndexSmallerThan(start);

  nat length = index; 
  if(length < donor->numElem && start <= donor->array[length]->absPos)
    {
      while( length < donor->numElem && donor->array[length]->absPos < end  )
	length++; 
    }
  length -= index; 
  
  if(length)
    {
      while(length + this->numElem > this->capacity)
	resize();
      memcpy(this->array + numElem, donor->array + index, length * sizeof(MUTATION*));
      numElem += length; 
    }
}


template<class MUTATION>
void SequenceArray<MUTATION>::claimMutations()
{
  for(nat i = 0; i < numElem; ++i)
    array[i]->claim();
}


