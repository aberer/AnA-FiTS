#include "../src/common.hpp"
#include <gtest/gtest.h>

TEST(commonTest, general)
{
  ASSERT_EQ(2,DIVIDE_2(4)); 
  ASSERT_EQ(32,MULT_16(2)); 
  ASSERT_EQ(1, MODULO_16(17)); 
}
