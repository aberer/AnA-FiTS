#include "SequenceArray.hpp"


template<> void SelectedArray::mutate(SelectedMutation &mut, bool replace) 
{
  fitness *= mut.fitness;
  assert(fitness != 0); 
  assert(this) ; 
  mutate_helper(mut, replace); 
  assert(fitness != 0); 
}

template<>		       
void NeutralArray::mutate(NeutralMutation &mut, bool replace) 
{
  mutate_helper(mut, replace); 
}


template<>
ostream& operator<<(ostream &stream, const SelectedArray &rhs)
{
  for(nat i = 0; i < rhs.numElem; ++i)
    stream << "[" << rhs.array[i]->absPos  
      << "," << rhs.array[i]->fitness
	   <<  "],"; 
  return stream; 
}


template<>
ostream& operator<<(ostream &stream, const NeutralArray &rhs)
{
  for(nat i = 0; i < rhs.numElem; ++i)
    {
      NeutralMutation *mut = rhs.array[i]; 
      stream << "[" << mut->absPos << "," << mut->base << "," << mut->generation <<  "],"; 
    }
  return stream; 
}


template<>
void NeutralArray::printSeq(FILE *fp) const
{
  nat number2 = size();
  BIN_WRITE(number2, fp); 
  nat c = 0; 
  for(auto mut : *this)
    {
      BIN_WRITE(mut->absPos,fp);
      // BIN_WRITE(mut->generation, fp); 
      c++; 
    }
  assert(c == number2); 
}


template<>
void SelectedArray::printSeq(FILE *fp) const 
{
  nat number2 = size(); 
  BIN_WRITE(number2,fp); 
  nat c = 0; 
  for(auto mut : *this)
    {
      BIN_WRITE(mut->absPos,fp); 
      FITNESS_TYPE tmp = mut->fitness-1; 
      BIN_WRITE(tmp,fp); 
      // BIN_WRITE(mut->generation, fp); 
      c++; 
    }
  assert(c == number2); 
}


// :TODO: does not resolve everything: what about two mutations in the same position in the same generation ?
template<>
void NeutralArray::conditionallyJoinOtherSeq(const NeutralArray &rhs, seqLen_t start, seqLen_t end)
{
  // am i large enough? 
  while(capacity < numElem + rhs.numElem)
    resize();
  
  NeutralMutation* tmpArray[ numElem + rhs.numElem ]; 

  nat
    indexNew = 0, 
    indexHere = 0,
    indexRhs = 0; 

  while(indexRhs < rhs.numElem && rhs.array[indexRhs]->absPos < start)
    indexRhs++; 
  
  while(indexRhs < rhs.numElem && indexHere < numElem
	&& rhs.array[indexRhs]->absPos < end )
    {
      NeutralMutation *rhsMut = rhs.array[indexRhs],
	*hereMut = array[indexHere]; 
      
      if(rhsMut->absPos < hereMut->absPos)
	tmpArray[indexNew++] = rhs.array[indexRhs++]; 
      else if(hereMut->absPos  < rhsMut->absPos)
	tmpArray[indexNew++] = array[indexHere++]; 
      else 	
	tmpArray[indexNew++] = rhsMut->generation > hereMut->generation ? rhs.array[indexRhs++] : array[indexHere++]; 
    }

  while(indexRhs < rhs.numElem && rhs.array[indexRhs]->absPos < end)
    tmpArray[indexNew++] = rhs.array[indexRhs++]; 

  while(indexHere < numElem)
    tmpArray[indexNew++] = array[indexHere++]; 

  numElem = indexNew; 
  memcpy(array, tmpArray, indexNew * sizeof(NeutralMutation*));
}


