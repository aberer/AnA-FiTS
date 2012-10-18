#include "common.hpp"

#include "BitSet.hpp"
#include "Base.hpp"

#include <sstream>
#include <string>



/** 
    This program can be used as an example of how to extract the
    binary information.  A program using binary information directly
    should be much faster.
 */ 


char toChar(Base base)
{
  switch(base)
    {
    case A:
      return 'A'; 
    case C: 
      return 'C' ; 
    case G:
      return 'G';
    case T:
      return 'T'; 
    default : 
      {
	assert(0); 
	return '0'; 
      }
      
    }
}



void helpMessage()
{
  cerr << "Usage: " 
       << "\tconvertSeq [-h|--help] [outfile] infile" << endl << endl
       << "\tinfile must be a file obtained by AnaFits (named"  << endl 
       << "\tanafits_polymorphisms.<id>). If no output file is provided, the " << endl
       << "\toutput is written to stdout (stdout + piping is usually less " << endl
       << "\tefficient than printing to a file)." << endl
       << endl; 
  exit(1);      
} 


void charToBinaryChar(nat num, char *&result)
{
  nat ctr = 0; 
  for(nat i = 1; i < 256 ; i *= 2)
    {
      if(num & i)
	result[ctr++] = '1'; 
      else 
	result[ctr++] = '0'; 
    }
}


void precomputeBvToChar(char **&result)
{
  result = (char**) calloc(256, sizeof(char*)); 
  for(nat i = 0; i < 256; ++i)
    result[i] = (char*) calloc(256,  sizeof(char)); 
  
  for(nat i = 0; i < 256; ++i)
    charToBinaryChar(i, result[i]); 
}


void printBitvectors(bool toStdout, FILE *ifh, FILE *ofh, char **precomputed, nat numMut)
{
  nat bvLength = BitSet<uint64_t>::convertToInternalSize(numMut);
  
  nat numOfSeq;
  BIN_READ(numOfSeq, ifh);
  uint64_t bv[bvLength];
  pun_t<uint64_t, uint8_t> converter;
 
  for(nat i = 0; i < numOfSeq; ++i)
    {
      if(toStdout)
	printf("%d\t", i);
      else 
	fprintf(ofh, "%d\t", i); 

      fread(bv, bvLength, sizeof(uint64_t), ifh);
      
      for(nat j = 0; j < bvLength - 1; ++j)
	{
	  converter.whole = bv[j]; 

	  if(toStdout)
	    printf("%s%s%s%s%s%s%s%s", 
	  	   precomputed[converter.part[0]],
	  	   precomputed[converter.part[1]],
	  	   precomputed[converter.part[2]],
	  	   precomputed[converter.part[3]],
	  	   precomputed[converter.part[4]],
	  	   precomputed[converter.part[5]],
	  	   precomputed[converter.part[6]],
	  	   precomputed[converter.part[7]]
	  	   );
	  else 
	    fprintf(ofh, "%s%s%s%s%s%s%s%s", 
	  	    precomputed[converter.part[0]],
	  	    precomputed[converter.part[1]],
	  	    precomputed[converter.part[2]],
	  	    precomputed[converter.part[3]],
	  	    precomputed[converter.part[4]],
	  	    precomputed[converter.part[5]],
	  	    precomputed[converter.part[6]],
	  	    precomputed[converter.part[7]]
	  	    ); 	  
	} 
      
      if(toStdout)
	printf("\n"); 
      else 
	fprintf(ofh, "\n"); 
    }  
}


void printMutations(FILE *ifh, FILE *ofh, bool toStdout, nat& numMut)
{ 
  nat nextNum = 0; 
  BIN_READ(nextNum, ifh); 

  for(nat j = 0; j < nextNum; ++j)
    {
      seqLen_t pos; 
      BIN_READ(pos, ifh); 
      nat generation; 
      BIN_READ(generation, ifh); 
      double fitness ; 
      BIN_READ(fitness, ifh); 
      Base base; 
      BIN_READ(base, ifh); 	  
      char baseRep[2]; 
      baseRep[0] = toChar(base);	  
      baseRep[1] = '\0'; 

      if(toStdout)
	printf("%d,%d,%f,%s;", pos, generation, fitness, baseRep); 
      else 
	fprintf(ofh, "%d,%d,%f,%s;", pos, generation, fitness, baseRep); 
    }

  numMut = nextNum; 
}


int main(int argc, char **argv)
{
  bool toStdout = false; 
  char
    *infile, 
    *outfile; 

  char **precomputedRep; 
  precomputeBvToChar(precomputedRep);

  if(argc == 2)
    {
      if(NOT strcmp(argv[1], "-h")  || NOT strcmp(argv[1], "--help"))
	helpMessage();
      
      toStdout = true; 
      infile = argv[1]; 
    }
  else if(argc == 3)
    {
      if(NOT strcmp(argv[1], "-h")  || NOT strcmp(argv[1], "--help"))
	helpMessage();
      infile = argv[2]; 
      outfile = argv[1]; 
    }
  else
    helpMessage();


  FILE *ifh = fopen(infile, "r"); 
  FILE *ofh = NULL ;

  if(NOT toStdout)
    ofh = fopen(outfile, "w"); 

  nat numChrom; 
  BIN_READ(numChrom,ifh); 
  for(nat i = 0; i < numChrom; ++i)
    {
      nat numMut = 0; 
      // print fixed mutations 
      if(toStdout)
	printf("// chromId %d\nfixed:\t", i); 
      else 
	fprintf(ofh,"// chromId %d\nfixed:\t", i);       
      printMutations(ifh, ofh, toStdout, numMut);


      // print mutations 
      if(toStdout)
	printf("\nmutations:\t"); 
      else 
	fprintf(ofh, "\nmutations:\t"); 
      printMutations(ifh, ofh, toStdout, numMut);
	  
      if(toStdout)
	printf("\n"); 
      else 
	fprintf(ofh, "\n"); 

      // print bitvectors
      printBitvectors(toStdout, ifh, ofh, precomputedRep, numMut);
    } 
}
