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
  stream << "[" << rhs.absPos << "," << rhs.generation << ","  
	 << rhs.base <<  "]"; 
  return stream; 
}


ostream& operator<<(ostream &stream, const SelectedMutation &rhs)
{
  stream << "[" << rhs.absPos << "," << rhs.generation << "," << rhs.fitness << "," << rhs.base <<  "]"; 
  return stream; 
}
