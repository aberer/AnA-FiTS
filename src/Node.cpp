#include "Node.hpp"

ostream& operator<<(ostream &stream, const NodeType &rhs)
{
  switch(rhs) 
    {
    case NOTHING:
      stream << "NOTHING" ;
      break; 
    case MUTATION:
      stream << "MUT"; 
      break; 
    case RECOMBINATION:
      stream << "REC"; 
      break; 
    case GENE_CONVERSION:
      stream << "G_C"; 
      break; 
    case COALESCENT: 
      stream << "COA"; 
      break; 
    default: 
      stream << "NOT IDENTIFIED" ; 
    }
  return stream; 
}


ostream& operator<<(ostream &stream, const Node &rhs)
{
  stream << 
    "***id=" << rhs.id  << "***"<< 
    ", indi=" << rhs.indiNr << 
    ", loc="<< rhs.loc<< 
    ", gen=" << rhs.originGen << 
    ", anc1="<< rhs.ancId1 << 
    ", anc2="<<rhs.ancId2 << 
    ", type=" << rhs.type; 
    
  return stream;
}


ostream& operator<<(ostream &stream, const NodeExtraInfo &rhs)
{
  stream
#ifndef USE_BVBASED_EXTRACTION
   << "seq=" << rhs.sequence
#endif
	 << ", start=" << rhs.start
	 << ", end=" << rhs.end
	 << ", refed=" << rhs.referenced
	 << ", wasInit="<< rhs.wasInitialized;  
  return stream; 
}


void Node::printRaw(FILE *fh)
{
  BIN_WRITE(id, fh);   
  BIN_WRITE(loc, fh); 
  BIN_WRITE(originGen, fh); 
  BIN_WRITE(base, fh);
  BIN_WRITE(ancId1, fh); 
  BIN_WRITE(ancId2, fh); 
  BIN_WRITE(type, fh); 

  // cout << id <<  "\t("
  //      << loc << ","
  //      << originGen << ","
  //      << base << ")\t"
  //      << ancId1 << ","
  //      << ancId2 << "\t"
  //      << type << endl; 
}
