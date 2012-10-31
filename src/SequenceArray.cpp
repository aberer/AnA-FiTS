#include "SequenceArray.hpp"


template<> void SelectedArray::mutate(SelectedMutation &mut, bool replace, bool insertAnyway, bool consistencyCheck) 
{
  assert(NOT (insertAnyway && replace) ); 
  fitness *= mut.fitness;
  assert(fitness != 0); 
  assert(this) ; 
  mutate_helper(mut, replace, insertAnyway, consistencyCheck);
  assert(fitness != 0); 
}

template<>		       
void NeutralArray::mutate(NeutralMutation &mut, bool replace, bool insertAnyway, bool consistencyCheck ) 
{
  assert(NOT (insertAnyway && replace) ); 
  mutate_helper(mut, replace, insertAnyway, consistencyCheck); 
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

