#include <gtest/gtest.h>
#include "../src/RegionManager.hpp"


TEST(RegionManagerTest, testScenario)
{
  nat maxPopSize = 1000; 
  seqLen_t seqLen  = 100000000; 

  RegionManager regMan(maxPopSize, seqLen); 
  regMan.extendPrevRegion(5, 100, 1000); 
  regMan.nextGenBackwards();
  bool survives = regMan.locusSurvivesInPresent(5, 101); 
  ASSERT_TRUE(survives);  
}
