#include "../src/SequenceArray.hpp"
#include <gtest/gtest.h>

TEST(SequenceArrayTest, mutate)
{
  SelectedMutation mut;
  mut.absPos = 123;
  mut.fitness = 1.0; 
  SelectedMutation mut2;
  mut2.absPos = 50;
  mut2.fitness = 1.0; 
  SelectedMutation mut3;
  mut3.absPos = 23;
  mut3.fitness = 1.0; 
  SelectedMutation mut4;
  mut4.absPos = 1234;
  mut4.fitness = 1.0; 
  
  SequenceArray<SelectedMutation> seq(1); 
  seq.mutate(mut2, true); 
  seq.mutate(mut, true ); 
  seq.mutate(mut4, true ); 
  seq.mutate(mut3, true ); 
  
  ASSERT_EQ(23U, seq[0]->absPos); 
  ASSERT_EQ(50U, seq[1]->absPos); 
  ASSERT_EQ(123U, seq[2]->absPos); 
  ASSERT_EQ(1234U, seq[3]->absPos); 
}


TEST(SequenceArrayTest, emptyRecombine)
{
  SequenceArray<SelectedMutation> seqA(3); 
  SequenceArray<SelectedMutation> seqB(3); 
  
  SequenceArray<SelectedMutation> recombinant(1); 
  
  Recombination mapRec[2]; 
  mapRec[0].absPos = 24; 
  mapRec[1].absPos = 70; 

  recombinant.recombine(seqA, seqB, mapRec, 2); 
  ASSERT_EQ(0U, recombinant.size()); 
}


TEST(SequenceArrayTest, testAssignment)
{
  SelectedMutation mut; 
  mut.absPos = 123; 
  mut.fitness = 1.0; 
  SelectedMutation mut2;   
  mut2.absPos = 50; 
  mut2.fitness = 1.0; 
  SelectedMutation mut3; 
  mut3.absPos = 23; 
  mut3.fitness = 1.0; 
  SelectedMutation mut4; 
  mut4.absPos = 1234; 
  mut4.fitness = 1.0; 
  
  SequenceArray<SelectedMutation> seq(1); 
  seq.mutate(mut, true ); 
  seq.mutate(mut2, true ); 
  seq.mutate(mut3, true );   
  seq.mutate(mut4, true ); 

  SequenceArray<SelectedMutation> seq2(1); 
  seq2 = seq; 
  ASSERT_EQ(4U, seq2.size()); 
  ASSERT_EQ(23U, seq2[0]->absPos); 
  ASSERT_EQ(50U, seq2[1]->absPos); 
  ASSERT_EQ(123U, seq2[2]->absPos); 
  ASSERT_EQ(1234U, seq2[3]->absPos);   
}



TEST(SequenceArrayTest, getIfPresent)
{  
  SelectedMutation mut; 
  mut.absPos = 123; 
  mut.fitness = 1.0; 
  SelectedMutation mut2;   
  mut2.absPos = 50; 
  mut2.fitness = 1.0; 
  SelectedMutation mut3; 
  mut3.absPos = 23; 
  mut3.fitness = 1.0; 
  SelectedMutation mut4; 
  mut4.absPos = 1234; 
  mut4.fitness = 1.0; 
  
  SequenceArray<SelectedMutation> seq(1); 
  seq.mutate(mut, true ); 
  seq.mutate(mut2, true); 
  seq.mutate(mut3, true );   
  seq.mutate(mut4, true); 

  ASSERT_TRUE(&mut2 == seq.getIfPresent(50)); 
  ASSERT_TRUE(&mut == seq.getIfPresent(123)); 
  ASSERT_TRUE(&mut3 == seq.getIfPresent(23)); 
  ASSERT_TRUE(&mut4 == seq.getIfPresent(1234)); 

  ASSERT_FALSE(seq.getIfPresent(1)); 
  ASSERT_FALSE(seq.getIfPresent(24)); 
  ASSERT_FALSE(seq.getIfPresent(13545)); 
}



TEST(SequenceArrayTest, searchTest)
{
  SelectedMutation mut; 
  mut.absPos = 123; 
  mut.fitness = 1.0; 
  SelectedMutation mut2;   
  mut2.absPos = 50; 
  mut2.fitness = 1.0; 
  SelectedMutation mut3; 
  mut3.absPos = 23; 
  mut3.fitness = 1.0; 
  SelectedMutation mut4; 
  mut4.absPos = 1234; 
  mut4.fitness = 1.0; 
  
  SequenceArray<SelectedMutation> seq(1); 
  seq.mutate(mut, true ); 
  seq.mutate(mut2, true); 
  seq.mutate(mut3, true );   
  seq.mutate(mut4, true); 

  ASSERT_EQ(0U,seq.searchIndexSmallerThan(0));
  ASSERT_EQ(0U,seq.searchIndexSmallerThan(22));
  ASSERT_EQ(1U,seq.searchIndexSmallerThan(50));
  ASSERT_EQ(2U,seq.searchIndexSmallerThan(122));
  ASSERT_EQ(3U,seq.searchIndexSmallerThan(126));
  ASSERT_EQ(4U,seq.searchIndexSmallerThan(12345));
  
}



TEST(SequenceArrayTest,searchTestExtreme)
{
  SelectedMutation
    m1, m2, m3, m4, m5, m6, m7, m8, m9, m10; 
  m1.absPos = 1 ;
  m2.absPos = 2 ;
  m3.absPos = 3 ;
  m4.absPos = 4 ;
  m5.absPos = 5 ;
  m6.absPos = 6 ;
  m7.absPos = 7 ;
  m8.absPos = 8 ;
  m9.absPos = 9 ;
  m10.absPos = 10 ;

  m1.fitness = 1 ;
  m2.fitness = 2 ;
  m3.fitness = 3 ;
  m4.fitness = 4 ;
  m5.fitness = 5 ;
  m6.fitness = 6 ;
  m7.fitness = 7 ;
  m8.fitness = 8 ;
  m9.fitness = 9 ;
  m10.fitness = 10 ;

  SequenceArray<SelectedMutation> seqA(10);  
  seqA.mutate(m1, true ) ; 
  seqA.mutate(m2, true) ; 
  seqA.mutate(m3, true ) ; 
  seqA.mutate(m4, true) ; 
  seqA.mutate(m5, true ) ; 
  seqA.mutate(m6, true) ; 
  seqA.mutate(m7, true ) ; 
  seqA.mutate(m8, true) ; 
  seqA.mutate(m9, true ) ; 
  seqA.mutate(m10, true) ; 

  ASSERT_EQ(0U, seqA.searchIndexSmallerThan(0));
  ASSERT_EQ(0U, seqA.searchIndexSmallerThan(1));
  ASSERT_EQ(1U, seqA.searchIndexSmallerThan(2));
  ASSERT_EQ(2U, seqA.searchIndexSmallerThan(3));
  ASSERT_EQ(3U, seqA.searchIndexSmallerThan(4));
  ASSERT_EQ(4U, seqA.searchIndexSmallerThan(5));
  ASSERT_EQ(5U, seqA.searchIndexSmallerThan(6));
  ASSERT_EQ(6U, seqA.searchIndexSmallerThan(7));
  ASSERT_EQ(7U, seqA.searchIndexSmallerThan(8));
  ASSERT_EQ(8U, seqA.searchIndexSmallerThan(9));
  ASSERT_EQ(9U, seqA.searchIndexSmallerThan(10));
  ASSERT_EQ(10U, seqA.searchIndexSmallerThan(11));  

}

TEST(SequenceArrayTest, removeTest)
{
  SequenceArray<SelectedMutation> seq(1); 

  SelectedMutation mut;
  mut.absPos = 123; 
  mut.fitness = 123; 
  SelectedMutation mut2; mut2.absPos = 2345; mut2.fitness = 2345; 
  SelectedMutation mut3; mut3.absPos = 1234; mut3.fitness = 1234; 

  seq.mutate(mut, true );
  seq.remove(&mut); 
  ASSERT_EQ(0U, seq.size()); 


  seq.mutate(mut, true); 
  seq.mutate(mut2, true ); 
  seq.mutate(mut3, true); 
  seq.remove(&mut); 
  ASSERT_EQ(2U, seq.size()); 
  ASSERT_EQ(1234U,seq.at(0)->absPos);
  ASSERT_EQ(2345U,seq.at(1)->absPos);

  seq.mutate(mut, true ); 
  seq.remove(&mut2); 

  ASSERT_EQ(2U, seq.size());   
  ASSERT_EQ(123U,seq.at(0)->absPos);
  ASSERT_EQ(1234U,seq.at(1)->absPos);


  seq.mutate(mut2, true); 
  seq.remove(&mut3); 

  ASSERT_EQ(2U, seq.size());   
  ASSERT_EQ(123U,seq.at(0)->absPos);
  ASSERT_EQ(2345U,seq.at(1)->absPos);  
}



TEST(SequenceArrayTest, recombine)
{
  // sequence 1     23, 50, 123, 1234
  SelectedMutation mut; 
  mut.absPos = 123; 
  mut.fitness = 123; 
  SelectedMutation mut2;   
  mut2.absPos = 50; 
  mut2.fitness = 50; 
  SelectedMutation mut3; 
  mut3.absPos = 23; 
  mut3.fitness = 23; 
  SelectedMutation mut4; 
  mut4.absPos = 1234; 
  mut4.fitness = 1234; 
  
  SequenceArray<SelectedMutation> seq(1); 
  seq.mutate(mut, true ); 
  seq.mutate(mut2, true); 
  seq.mutate(mut3, true ); 
  seq.mutate(mut4, true); 

  // sequence 2    1, 25, 80
  SequenceArray<SelectedMutation> seq2(1); 
  SelectedMutation mut5; 
  mut5.absPos = 1; 
  mut5.fitness = 1; 
  SelectedMutation mut6; 
  mut6.absPos = 25; 
  mut6.fitness = 25; 
  SelectedMutation mut7; 
  mut7.absPos = 80; 
  mut7.fitness = 80; 

  seq2.mutate(mut5, true ); 
  seq2.mutate(mut6, true); 
  seq2.mutate(mut7, true ); 

  ASSERT_EQ(1U, seq2[0]->absPos); 
  ASSERT_EQ(25U, seq2[1]->absPos); 
  ASSERT_EQ(80U, seq2[2]->absPos); 

  // 0-24-70-end
  Recombination mapRec[2]; 
  mapRec[0].absPos = 24; 
  mapRec[1].absPos = 70; 
  
  // should be  23,25,123,1234
  SequenceArray<SelectedMutation> recombinant(1); 
  recombinant.recombine(seq, seq2, mapRec, 2); 
  
  ASSERT_EQ(23U, recombinant[0]->absPos); 
  ASSERT_EQ(25U, recombinant[1]->absPos); 
  ASSERT_EQ(123U, recombinant[2]->absPos); 
  ASSERT_EQ(1234U, recombinant[3]->absPos); 
  ASSERT_EQ(4U, recombinant.size()); 

  SelectedMutation mut123; 
  mut123.absPos = 13; 
  mut123.fitness = 13; 
  recombinant.mutate(mut123, true);
  ASSERT_EQ(5U, recombinant.size());   
}


TEST(SequenceArrayTest, testJoin)
{
  NeutralArray seqA(10),
    seqB(10); 

  NeutralMutation
    mut1({1,1000, Base::A, false}),
    mut123 ({123,1000, Base::A, false}),
    mut245({245, 1000, Base::T, false}), 
    mut150({150, 1000, Base::T, false}), 
    mut76({76, 10000, Base::T, false}); 
  
  seqA.mutate(mut123, true ); 
  seqA.mutate(mut150, true);   

  seqB.mutate(mut76, true ); 
  seqB.mutate(mut245, true); 

  seqA.conditionallyJoinOtherSeq(seqB,0,-1); 
  NeutralMutation** array = seqA.begin();
  ASSERT_EQ(76U, array[0]->absPos); 
  ASSERT_EQ(123U, array[1]->absPos); 
  ASSERT_EQ(150U, array[2]->absPos); 
  ASSERT_EQ(245U, array[3]->absPos); 

  NeutralArray seqC(10); 
  seqC.mutate(mut1, true ); 
  seqC.conditionallyJoinOtherSeq(seqA,130,250); 
  array = seqC.begin();
  ASSERT_EQ(1U, array[0]->absPos); 
  ASSERT_EQ(150U, array[1]->absPos); 
  ASSERT_EQ(245U, array[2]->absPos);
}
