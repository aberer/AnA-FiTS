#include "../src/BitSet.hpp"
#include "../src/common.hpp"
#include <gtest/gtest.h>


// COPY will of course fail, if you disallow it
TEST(bitset, general)
{
  // flipping
  BitSet<uint64_t> bs4(30); 
  bs4.flip(1);   
  ASSERT_TRUE(bs4.test(1)); 
  bs4.flip(1); 
  ASSERT_TRUE(NOT bs4.test(1)); 
  
  // copying 
  BitSet<uint64_t> bs(1000); 
  bs.set(300); 
  BitSet<uint64_t> bs2(1000); 
  bs2 =  bs ; 
  ASSERT_TRUE(bs2.test(300));   
  bs2.set(400); 
  ASSERT_TRUE(bs2.test(400)); 
  ASSERT_TRUE( NOT bs.test(400) ); 

  // unsetting 
  bs.unset(300) ; 
  ASSERT_TRUE(NOT bs.test(300)) ; 

}


TEST(BitSetTest, fullByteBv)
{
  BitSet<uint64_t> bs1(128),
    bs2(128); 
  
  bs1.set(1); 
  bs1.set(2); 
  
  bs2.set(3); 
  bs2.set(4); 

  bs1.orify(bs2); 
  ASSERT_EQ(4U,bs1.count());
  
}


TEST(BitSetTest, orifyRange)
{
  BitSet<uint64_t> bs1(200); 
  BitSet<uint64_t> bs2(200); 
  bs2.set();

  bs1.unset();
  bs1.orifyPart(bs2,2,9);
  ASSERT_EQ(7U, bs1.count());  

  bs1.unset();
  bs1.orifyPart(bs2,33,65);
  ASSERT_EQ(32U, bs1.count());

  bs1.unset();  
  bs1.orifyPart(bs2, 64, 129);
  ASSERT_EQ(65U, bs1.count()); 

  bs1.unset();
  bs1.orifyPart(bs2, 0,200); 
  ASSERT_EQ(200U, bs1.count());   

}



TEST(BitSetTest, orifyRange2)
{
  BitSet<uint64_t> bs3(9280);
  BitSet<uint64_t> bs4(9280);
  bs4.set();

  bs3.unset();
  bs3.orifyPart(bs4,123,567); 
  ASSERT_EQ(444U,bs3.count());

  bs3.unset();
  bs3.orifyPart(bs4,0,9280); 
  ASSERT_EQ(9280U,bs3.count());
}



TEST(BitSetTest, copyRange)
{
  BitSet<uint64_t> bs1(32); 

  bs1.set();
  // ASSERT_EQ(32U, bs1.count()); 
  
  BitSet<uint64_t> bs2(32); 
  bs2.copyRange(bs1, 0, 10);
  
  for(nat i = 0; i < 10; ++i)
    ASSERT_TRUE(bs2.test(i));
  for(nat i = 10; i < 32; ++i)
    ASSERT_FALSE(bs2.test(i)); 

  bs2.unset();
  ASSERT_EQ(0U, bs2.count() );
  bs2.copyRange(bs1, 0,3); 
  ASSERT_EQ(3U, bs2.count()); 

  bs2.unset();
  bs2.copyRange(bs1, 0,32); 
  ASSERT_EQ(32U, bs2.count()); 
 
  bs2.unset();
  bs2.copyRange(bs1, 6,12);   
  ASSERT_EQ(6U, bs2.count()); 

  bs2.unset();
  bs2.copyRange(bs1, 3,6);   
  ASSERT_EQ(3U, bs2.count());  

  bs2.unset();
  bs2.copyRange(bs1,23,31); 
  ASSERT_EQ(8U, bs2.count());  

  bs2.unset();
  bs2.copyRange(bs1,7,30); 
  ASSERT_EQ(23U, bs2.count());  

  BitSet<uint64_t> bs3(1000),
    bs4(1000); 
  bs3.set(38); 
  bs3.set(32); 
  
  bs4.copyRange(bs3,32,40); 
  ASSERT_TRUE(bs4.test(38)); 
  ASSERT_TRUE(bs4.test(32)); 
}


TEST(BitSetTest,copyLongRange)
{
  BitSet<uint64_t> bs1(1000); 
  bs1.set(); 

  BitSet<uint64_t> bs2(1000); 
  bs2.copyRange(bs1, 31, 87); 

  for(nat i = 0; i <  31; ++i)
    ASSERT_FALSE(bs2.test(i)); 
  for(nat i = 31; i < 87; ++i)
    ASSERT_TRUE(bs2.test(i)); 
  for(nat i = 87; i < 1000; ++i )
    ASSERT_FALSE(bs2.test(i));     
  ASSERT_EQ(56U, bs2.count()); 

  BitSet<uint64_t> bs3(1000); 
  bs3.copyRange(bs1, 23,567); 
  ASSERT_EQ(544U, bs3.count());

}
