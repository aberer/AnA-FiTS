#pragma once 
#include "common.hpp"
#include "types.hpp"
#include "HaploTimeWindow.hpp"
#include "Chromosome.hpp"
#include "Graph.hpp"

#include <string>

class BinaryWriter
{
public: 
  BinaryWriter(string fileName, nat numChromForSeq, nat numGraph);
  ~BinaryWriter();
  void writeGraph(Graph &graph); 
  void writeSequences(Graph &graph, HaploTimeWindow &haplo,  Chromosome &chrom ); 
  
private: 
  FILE *fh; 
}; 



