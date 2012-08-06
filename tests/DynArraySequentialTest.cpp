#include <gtest/gtest.h>
#include "../src/DynArraySequential.hpp"

TEST(DynArraySequentialTest, resize)
{
  DynArraySequential<nat> array (3) ; 

  array.setNext(1); 
  array.setNext(2); 
  array.setNext(3); 
  array.setNext(4);
  
  ASSERT_EQ(1,(int)array.at(0)) ; 
  ASSERT_EQ(2,(int)array.at(1)) ; 
  ASSERT_EQ(3,(int)array.at(2)) ; 
  ASSERT_EQ(4,(int)array.at(3)) ; 
} 



TEST(DynArraySequentialTest, popTest)
{
  DynArraySequential<nat> array(3); 

  nat ints[10] = {1,2,3,4,5,6,7,8,9,10}; 
  
  for(nat i = 0; i < 10; i++)
    array.setNext(ints[i]); 

  for(nat i = 0; i < 10; ++i)
    {
      nat& elem = array.pop(); 
      ASSERT_EQ(10 - i, elem); 
    }

  for(nat i = 0; i < 10; ++i)
    array.setNext(ints[i]);


  for(nat i = 0; i < 10; ++i)
    {
      nat& elem = array.pop(); 
      ASSERT_EQ(10 - i, elem); 
    }  
}

