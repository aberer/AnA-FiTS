#pragma once 

#include "common.hpp"
#include "types.hpp"
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
  NeutralArray *sequence; 
  seqLen_t start; 
  seqLen_t end;
  nat referenced; 
  bool wasInitialized;
  bool skip;
}; 



ostream& operator<<(ostream &stream, const NodeType &rhs); 
ostream& operator<<(ostream &stream, const Node &rhs); 
ostream& operator<<(ostream &stream, const NodeExtraInfo &rhs); 
