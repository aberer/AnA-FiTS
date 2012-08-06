#include <gtest/gtest.h>
#include "../src/Survivors.hpp"

#define popSize 100
#define seqLen 100000000 
#define numGen 1000


// :TODO: this class is dysfunctional. The reason is that currently
// the bitset is not ressized in the survivorclass

// class SurvivorsTest : public ::testing::Test 
// {
// public:  
//   Survivors survivors; 
  
//   SurvivorsTest() :
//     survivors(5, numGen )
//   {
//     survivors.prepareNextGenBackwards();

//     survivors.addSurvivor(3);
//     survivors.addSurvivor(4);
//     survivors.addSurvivor(24);
//     survivors.addSurvivor(3);     
    
//     testBaseInClass();
//   }

//   void testBaseInClass()
//   {
//     nat *iter = &(survivors.survivors[0]);
//     ASSERT_EQ(0U, *iter); 
//     iter++;
//     ASSERT_EQ(1U, *iter); 
//     iter++;
//     ASSERT_EQ(2U, *iter); 
//     iter++;
//     ASSERT_EQ(3U, *iter); 
//     iter++;
//     ASSERT_EQ(4U, *iter); 
//     iter++;
//   }


//   void testAddSurvivorInClass()
//   {
//     ASSERT_TRUE(&(survivors.survivors[8]) == survivors.nextSurvivor); 
//     ASSERT_EQ(3U, survivors.survivors[5]); 
//     ASSERT_EQ(4U, survivors.survivors[6]); 
//     ASSERT_EQ(24U, survivors.survivors[7]); 
//   }

//   void testNextGenerationInClass()
//   {
//     survivors.prepareNextGenBackwards();
//     nat *tmp = survivors.getFirstSurvivorBackwards(1); 
//     if(tmp)
//       ASSERT_EQ(3U, *tmp); 
//     else 
//       ASSERT_TRUE(0); 
//   }

//   void testNextNextGenerationInClass()
//   {
//     survivors.prepareNextGenBackwards();
//     nat *tmp = survivors.getFirstSurvivorBackwards(1); 
//     if(tmp)
//       ASSERT_EQ(3U, *tmp); 
//     else
//       ASSERT_TRUE(0); 

//     survivors.addSurvivor(20); 
//     survivors.addSurvivor(3); 
//     survivors.addSurvivor(30); 
    
//     survivors.prepareNextGenBackwards();
//     tmp = survivors.getFirstSurvivorBackwards(2); 
//     if(tmp)
//       ASSERT_EQ(20U, *tmp); 
//     else
//       ASSERT_TRUE(0); 
//   }

//   void testForwardInClass()
//   {
//     survivors.prepareNextGenBackwards();
//     survivors.addSurvivor(87); 
//     survivors.addSurvivor(78); 
//     survivors.addSurvivor(79); 
//     survivors.prepareNextGenBackwards();

//     nat *first = survivors.getFirstSurvivorForward(numGen-2); 
//     ASSERT_EQ(3U, *first);
//     first++;
//     ASSERT_EQ(4U, *first);
//     first++;
//     ASSERT_EQ(24U, *first);

//     first = survivors.getFirstSurvivorForward(numGen-3); 
//     ASSERT_EQ(87U ,  *first );
//     first++; 
//     ASSERT_EQ(78U, *first);
//     first++; 
//     ASSERT_EQ(79U, *first);
//   }
  
// } ;

// TEST_F(SurvivorsTest, testAddSurvivor)
// {
//   testAddSurvivorInClass(); 
// }

// TEST_F(SurvivorsTest, testNextGeneration)
// {
//   testNextGenerationInClass(); 
// }

// TEST_F(SurvivorsTest, testNextNextGeneration)
// {
//   testNextNextGenerationInClass();
// }

// TEST_F(SurvivorsTest, testForward)
// {
//   testForwardInClass();
// }
