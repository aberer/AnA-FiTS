#include "BinaryWriter.hpp"
#include "SequenceFinalizer.hpp"


BinaryWriter::BinaryWriter(string fileName, nat numChromForSeq, nat numGraph)
{
  fh = openFile(fileName, "w" );
  
  if(numChromForSeq && numGraph)
    abort();
  
  nat numToWrite = numChromForSeq == 0 ? numGraph : numChromForSeq; 
  BIN_WRITE(numToWrite,fh); 
}


BinaryWriter::~BinaryWriter()
{
  fclose(fh); 
}


void BinaryWriter::writeGraph(Graph& graph)
{
  graph.printRaw(fh);
}


void BinaryWriter::writeSequences(Graph &graph, HaploTimeWindow &haplo, Chromosome &chrom )
{
  vector<BitSetSeq*> rawNeutralSequences; 
  graph.getRawSequences(rawNeutralSequences); 
  vector<Node*>& rawBvMeaning = graph.getBvMeaning();
  vector<SelectedArray*> selectedSeqs; 

  // assert(rawNeutralSequences.size() == numHaplo); 
  for(nat i = 0; i < rawNeutralSequences.size(); ++i)      
    selectedSeqs.push_back(haplo.at(i)); 
  
  // a lot of preprocessing to get useful bitvectors 
  SequenceFinalizer finalizer;
  finalizer.computeFinalSequences(rawNeutralSequences, rawBvMeaning, selectedSeqs, 
				  chrom.getFixedMutations(0) // TODO pops
				  );      

  finalizer.printBinary(fh);
}
