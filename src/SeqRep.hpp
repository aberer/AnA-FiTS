#pragma once 

#include "BitSet.hpp"
#include "Mutation.hpp" 
#include "Randomness.hpp"

class SeqRep : public BitSet<uint8_t>
{  
public:
  // LIFE CYCLE
  explicit SeqRep(seqLen_t length);

  // OPERATORS
  void initSequence(Randomness &rng); 
  
  // ACCESSORS
  Base getBase(seqLen_t absPos) const;
  seqLen_t getLength() const {return this->numElems; }    
  friend ostream& operator<<(ostream &stream, const SeqRep &rhs); 
}; 




////////////////////
// IMPLEMENTATION //
////////////////////
inline Base SeqRep::getBase(seqLen_t absPos) const
{
  seqLen_t tmp = MULT_2(absPos); 

  uint8_t num = test(tmp ) | (test(tmp + 1  ) << 1 ); 
  return Base(num); 
}
