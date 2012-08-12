#include "config.hpp"
#include "Simulation.hpp"
#include "StopWatch.hpp"
#include "System.hpp"
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


int main(int argc, char **argv)
{
  System sys;
  StopWatch watch ; 
  watch.start();
  ProgramOptions opts(argc,argv); 
  
  setlocale(LC_ALL,"C");

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
  Simulation sim(opts);
  sim.run();  
#endif 
#ifdef PROFILE  
  ProfilerStop();
#endif
#ifdef INFO_GENERAL
  // cerr << "memory usage " << sys.getMaxMemConsumption() / 1024. <<   " kB" << endl; 
  cerr << "elapsed time " <<  watch.getElapsed() <<  endl ; // "\t"  <<  watch.getElapsedAsString() << endl ; 
#endif
}


