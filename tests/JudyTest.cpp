#include <gtest/gtest.h>
#include "../src/JudyArray.hpp"

TEST(judy, general)
{
  // intention: test as position mapper (selected sites)
 
  JudyArray<unsigned long, unsigned long> test;
  
  test.set(20, 0);
  test.set(40, 1); 
  test.set(80, 2); 
  test.set(90, 3); 
  test.set(100, 4); 

  ASSERT_EQ( nullptr,  test.get(123) ) ;
  ASSERT_TRUE(0 == *(test.get(20))); 
  ASSERT_TRUE(1==*(test.get(40))); 
  ASSERT_TRUE(2==*(test.get(80)) ); 
  ASSERT_TRUE(3==*(test.get(90))); 
  ASSERT_TRUE(4==*(test.get(100))); 
  
  unsigned long result = 0;   
  ASSERT_TRUE(test.getNextHighest(19, result));  
  ASSERT_EQ(0, int(result)); 
  ASSERT_TRUE(test.getNextHighest(40, result));  
  ASSERT_EQ(1, int(result)); 
  ASSERT_TRUE(test.getNextHighest(100, result));  
  ASSERT_EQ(4, int(result)); 
  ASSERT_FALSE(test.getNextHighest(200,result)); 
}
