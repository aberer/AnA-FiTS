#include "Mutation.hpp"

#include <cassert>


ostream& operator<<(ostream &stream, const Base base)
{
  switch(base)
    {
    case Base::A: 
      stream << "A" ;  
      break; 
    case Base::T: 
      stream << "T"  ; 
      break; 
    case Base::G: 
      stream << "G"  ;
      break; 
    case Base::C: 
      stream << "C"  ;
      break; 
    default: assert(0); 
    }
  return stream; 
}


ostream& operator<<(ostream &stream, const NeutralMutation &rhs)
{
  stream <<  rhs.absPos << "," << rhs.generation << ",0," << rhs.base << ";"; 
  return stream; 
}


ostream& operator<<(ostream &stream, const SelectedMutation &rhs)
{
  stream << rhs.absPos << "," << rhs.generation << "," << 1- rhs.fitness << "," << rhs.base <<  ";" ; 
  return stream; 
}


void SelectedMutation::printRaw(FILE *fh)
{
  BIN_WRITE(absPos,fh);
  BIN_WRITE(generation,fh);
  double fn = fitness  -1;
  BIN_WRITE(fn,fh);
  uint8_t tmp = uint8_t(base );
  BIN_WRITE(tmp,fh);
}
 

#include <iostream>
void NeutralMutation::printRaw(FILE *fh)
{
  BIN_WRITE(absPos,fh);  
  BIN_WRITE(generation,fh);
  double fitness = 0;  
  BIN_WRITE(fitness,fh);
  uint8_t tmp = uint8_t(base );
  BIN_WRITE(tmp,fh);
}


