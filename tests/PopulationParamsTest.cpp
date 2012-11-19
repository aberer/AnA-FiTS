#include "../src/ProgramOptions.hpp"
#include "../src/common.hpp"
#include "../src/PopulationParams.hpp"

#include <gtest/gtest.h>
#include <boost/algorithm/string.hpp>


#ifndef PRODUCTIVE


ProgramOptions *stringHelper(string commandLine)
{
  vector<string> tok; 
  boost::split(tok, commandLine, boost::is_any_of(" "));

  char **argv = (char**)calloc(tok.size(), sizeof(char*)) ; 
  
  for(nat i = 0; i < tok.size() ; ++i)
    argv[i] = const_cast<char*> (tok[i].c_str());
  
  ProgramOptions *result = new ProgramOptions(tok.size(), argv); 
  return result; 
}


TEST(PopulationParamsTest, noEvent)
{
  ProgramOptions *progOpt = stringHelper("./progname -s 1 -n tmp");

  nat popSize = 100; 
  PopulationParams popParam(0, popSize * 10 , popSize, *progOpt);  

  for(nat i = 0; i < popSize * 10; ++i )
    ASSERT_EQ(popSize, popParam.getPopSizeByGeneration(i)); 
  
  // ASSERT_EQ(100U, popParam.getMaximumPopSize()); 
}

TEST(PopulationParamsTest, testConstant)
{  
  ProgramOptions *progOpt = stringHelper("./progname -s 1 -T c 101 1  -n tmp");

  nat popSize = 100; 
  PopulationParams popParam(0, popSize * 10 , popSize, *progOpt);  

  for(nat i = 0; i < popSize * 10; ++i )
    ASSERT_EQ(popSize, popParam.getPopSizeByGeneration(i)); 
  
  // ASSERT_EQ(100U, popParam.getMaximumPopSize()); 
}

TEST(PopulationParamsTest, testLogistic)
{
  ProgramOptions *progOpt = stringHelper("./progname -s 1 -T k 101 1000 0.013  -n tmp");

  nat popSize = 100; 
  PopulationParams popParam(0, 200 , popSize, *progOpt);  
  ASSERT_EQ(150U, popParam.getPopSizeByGeneration(137)) ; 
}


TEST(PopulationParamsTest, testExponentialDecay)
{  
  ProgramOptions *progOpt = stringHelper("./progname -s 1 -T d 100 0.013  -n tmp");

  nat popSize = 100; 
  PopulationParams popParam(0, 200 , popSize, *progOpt);  

  ASSERT_EQ(32U, popParam.getPopSizeByGeneration(184)); 
}


TEST(PopulationParamsTest, testExponentialGrowth)
{  
  ProgramOptions *progOpt = stringHelper("./progname -s 1 -T g 100 0.013  -n tmp");

  nat popSize = 100; 
  PopulationParams popParam(0, 200 , popSize, *progOpt);  
  
  ASSERT_EQ(128U, popParam.getPopSizeByGeneration(120)); 
  ASSERT_EQ(264U, popParam.getPopSizeByGeneration(175));

}

#endif
