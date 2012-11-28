#include "../src/common.hpp"
#include <gtest/gtest.h>

TEST(productTest,general)
{
#ifndef  PRODUCTIVE
  ASSERT_EQ(0,1); 
#endif
}

TEST(commonTest, general)
{
  ASSERT_EQ(2,DIVIDE_2(4)); 
  ASSERT_EQ(32,MULT_16(2)); 
  ASSERT_EQ(1, MODULO_16(17)); 
}
