#include "config.hpp"
#include "Simulation.hpp"
#include "StopWatch.hpp"
#include "System.hpp"
#include "InfoFile.hpp"
#include "ProgramOptions.hpp"


#ifdef PROFILE
#include <google/profiler.h>
#endif



#ifdef _TEST
#include "Randomness.hpp"
void playground(ProgramOptions &progOpt){  cerr << "PLAYGROUND MODE " << endl;
  vector<nat> bla;
  bla.push_back(3); 

  nat next = 0; 
  
  for(auto iter = bla.begin(); iter != bla.end(); ++iter)
    if(next < *iter)
      {
	bla.insert(iter, next);
	break; 
      }

  for(auto tmp : bla )
    cout << tmp << "," ;   
  cout << endl; 
}
#endif


void printInfo(InfoFile &info, int argc, char **argv)
{
  info.write("This is AnA-FiTS version %g released by Andre J. Aberer\n", float(VERSION)); 
  info.write("For any inquiries, please send a mail to andre <dot> aberer <at> h-its <dot> org.\n\n"); 

  info.write("AnA-FiTS was called as\n\t"); 
  for(int i = 0; i < argc; ++i)
    info.write("%s ", argv[i]); 
  info.write("\nusing default parameters otherwise (see -h).\n\n");   
}



int main(int argc, char **argv)
{
  System sys;
  StopWatch watch ; 
  watch.start();
  ProgramOptions opts(argc,argv); 
  InfoFile info(opts.get<string>("runId")); 
  printInfo(info, argc, argv); 

  // if( opts.get<nat>("ploidy") != 2 ) 
  //   {
  //     cerr << "not implemented yet." << endl; 
  //     abort(); 
  //   }

#ifdef PROFILE 
  ProfilerStart("ffits.prof");
#endif
#ifdef _TEST 
  playground(opts);
#else 
  Simulation sim(info, opts);
  sim.run();  
#endif 
#ifdef PROFILE  
  ProfilerStop();
#endif
  
  double elapsed = watch.getElapsed();
  info.write("elapsed time %g\n", elapsed); 
#ifdef INFO_GENERAL
  cerr << "elapsed time " <<  elapsed <<  endl ; 
#endif
}

