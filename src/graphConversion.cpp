#include "common.hpp"

#include "Node.hpp"
#include "BitSet.hpp"
#include "Base.hpp"

#include <sstream>
#include <string>


void helpMessage()
{
  cerr << "Usage: " 
       << "\tconvertGraph [-h|--help] [outfile] infile" << endl << endl
       << "\tinfile must be a file obtained by AnaFits (named"  << endl 
       << "\tanafits_graph.<id>). If no output file is provided, the " << endl
       << "\toutput is written to stdout (stdout + piping is usually less " << endl
       << "\tefficient than printing to a file)." << endl
       << endl; 
  exit(1);      
}


// char toChar(Base base)
// {
//   switch(base)
//     {
//     case A:
//       return 'A'; 
//     case C: 
//       return 'C' ; 
//     case G:
//       return 'G';
//     case T:
//       return 'T'; 
//     default : 
//       {
// 	assert(0); 
// 	return '0'; 
//       }
      
//     }
// }





// #define MUTTATION 2 
// #define RECOMBINATION 3   


int main(int argc, char **argv)
{
  bool toStdout = false; 
  char
    *infile, 
    *outfile; 
  char c[2]; 

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
      fprintf(toStdout ? stdout : ofh, "// GRAPH %d\n", i); 

      nat num, tmp ; 
      BIN_READ(num, ifh);	// number of nodes 

      fprintf(toStdout ? stdout : ofh, "// survivingNodes "); 
      for(nat j = 0; j < num; ++j)
	{
	  BIN_READ(tmp, ifh); 
	  fprintf(toStdout ? stdout : ofh, "%d;", tmp); 
	}
      fprintf(toStdout ? stdout : ofh, "\n"); 

      BIN_READ(num, ifh); 
      readStruct allNodes[num];       
      fread(&allNodes, num, sizeof(readStruct), ifh); 

      for(nat j = 0; j < num; ++j)
	{
	  readStruct tmp = allNodes[j]; 
	  switch(tmp.type)
	    {
	    case MUTATION : 
	      {
		const char *c = BaseString[tmp.base]; 
	      
		fprintf(toStdout ? stdout : ofh,  "%d\t(%d,%d,%s)\t%d\n", tmp.id, tmp.loc, tmp.originGen, c, tmp.anc1); 
		break; 
	      }
	    case RECOMBINATION: 
	      {
		fprintf(toStdout ? stdout : ofh,  "%d\t(%d,%d)\t%d,%d\n", tmp.id, tmp.loc, tmp.originGen, tmp.anc1, tmp.anc2); 
	      }
	      break; 
	    case NOTHING : 
	    case GENE_CONVERSION:  	      
	    case COALESCENT:
	      cerr << "encountered unused node type" << endl; 
	      break; 
	    default: 
	      {
		// cerr << "unknown node type, please report this as bug and tell the developer, I read a" << endl; 
		cerr << "conversion failed, please file a bug report." << endl; 
		abort(); 
	      }
	    }
	}
    }
}

