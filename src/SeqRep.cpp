#include "SeqRep.hpp"

#include <iostream>


#define BITS_PER_BASE 2 

SeqRep::SeqRep(seqLen_t length) 
  : BitSet(BITS_PER_BASE * length)
{
}


void SeqRep::initSequence(Randomness &rng)
{
  rng.initArray(this->bv, internalSize()); 
}


ostream& operator<<(ostream &stream, const SeqRep &rhs)
{
  assert(BITS_PER_BASE == 2); 
  nat seqLen = DIVIDE_2(rhs.numElems );

  for(uint64_t c = 0; c < seqLen; ++c)
    stream << rhs.getBase(c);
  return stream; 
}
