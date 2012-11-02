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
  if(rhs.type == NodeType::RECOMBINATION)
    {
      stream
    << rhs.id << "\t" << "(" << rhs.loc <<  ","  <<  rhs.originGen
    << ","<< rhs.indiNr <<  ")\t"   << rhs.ancId1 << "," << rhs.ancId2;   
    }
  else if( rhs.type == NodeType::MUTATION)
    stream
      << rhs.id << "\t" << "(" << rhs.loc <<  ","  <<  rhs.originGen
      <<  "," << BaseString[rhs.base]<< "," << rhs.indiNr <<  ")\t"  
      << rhs.ancId1;   
  else 
    assert(0); 
  
  // stream << 
  //   "***id=" << rhs.id  << "***"<< 
  //   ", indi=" << rhs.indiNr << 
  //   ", loc="<< rhs.loc<< 
  //   ", gen=" << rhs.originGen << 
  //   ", anc1="<< rhs.ancId1 << 
  //   ", anc2="<<rhs.ancId2 << 
  //   ", type=" << rhs.type; 
    
  return stream;
}


ostream& operator<<(ostream &stream, const NodeExtraInfo &rhs)
{
  stream
	 << ", start=" << rhs.start
	 << ", end=" << rhs.end
	 << ", refed=" << rhs.referenced
	 << ", wasInit="<< rhs.isInitialized();  
  return stream; 
}


void Node::printRaw(FILE *fh)
{
  readStruct tmp; 
  
  tmp.id = id; 
  tmp.loc = loc; 
  tmp.originGen = originGen; 
  tmp.anc1 = ancId1; 
  tmp.anc2 = ancId2; 
  tmp.base = base; 
  tmp.type = type;   
  
  fwrite(&tmp, 1, sizeof(readStruct), fh);

  // cout << id <<  "\t("
  //      << loc << ","
  //      << originGen << ","
  //      << base << ")\t"
  //      << ancId1 << ","
  //      << ancId2 << "\t"
  //      << type << endl; 
}
