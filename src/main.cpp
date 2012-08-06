#include "config.hpp"
#include "Simulation.hpp"
// #include "CycleCounter.hpp"
#include "StopWatch.hpp"
#include "System.hpp"
#include "ProgramOptions.hpp"

#ifdef PROFILE
#include <google/profiler.h>
#endif

#ifdef _TEST
#include "Randomness.hpp"
void playground(ProgramOptions &progOpt){  cerr << "PLAYGROUND MODE " << endl;   
  Randomness rng(123); 
  
  uint16_t *buf = (uint16_t*)malloc(100000 * sizeof(uint16_t));  
  rng.IntegerArray(buf, 100000, 500);
  
  for(nat i = 0; i < 100000; ++i)
    cout << int(buf[i]) << endl; 
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


