#pragma once 

#include "common.hpp"
#include "types.hpp"
#include "Base.hpp"
#include "BitSet.hpp"
#include "SequenceArray.hpp"

#include <fstream> 

enum NodeType : uint8_t { 
  NOTHING = 0, 
    START = 1,     
    MUTATION = 2,
    RECOMBINATION = 3, 
    GENE_CONVERSION = 4, 
    COALESCENT = 5
    };


struct Node
{
  nat id; 
  nat indiNr;			// DONT PRINT 

  seqLen_t loc;
  nat originGen; 
  Base base; 

  nat ancId1; 
  nat ancId2; 

  NodeType type;

  void printRaw(FILE *fh);   
};



/** 
    Will be calloc-constructed. However, start must not be 0,
    therefore we need the wasInitialized-bool.
 */
struct NodeExtraInfo
{
#ifdef USE_BVBASED_EXTRACTION
  BitSet<uint64_t> *bv; 
#else 
  NeutralArray *sequence; 
#endif
  seqLen_t start; 
  seqLen_t end;

  // the following stuff could be compressed into a byte 
  nat referenced; 
  bool wasInitialized;
  bool skip;
  bool invertedOrientation; 

#ifdef USE_BVBASED_EXTRACTION
  nat bvIdx; 
#endif
}; 


ostream& operator<<(ostream &stream, const NodeType &rhs); 
ostream& operator<<(ostream &stream, const Node &rhs); 
ostream& operator<<(ostream &stream, const NodeExtraInfo &rhs); 
