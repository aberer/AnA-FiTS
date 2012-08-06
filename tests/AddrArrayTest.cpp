#include "../src/AddrArray.hpp"
#include "../src/common.hpp"
#include "../src/types.hpp"

#include <gtest/gtest.h>

TEST(AddrArrayTest,testIter)
{
  AddrArray<nat,true> test(3); 
  
  for(nat i = 0; i < 100; ++i)
    {
      nat *bla = test.getNext();
      *bla = i; 
    }

  AddrArrayBackwardIter<nat,true> iter;
  test.getEnd(iter); 
  
  bool canGoBack = true; 
  nat i = 99; 
  while(canGoBack)
    {
      // cout << i << endl; 
      nat* content = iter(); 
      ASSERT_EQ(i,*content);
      canGoBack = iter.back();
      --i;
    }
}
