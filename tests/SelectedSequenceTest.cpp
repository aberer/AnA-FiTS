#include <gtest/gtest.h>
#include "../src/SequenceArray.hpp"
#include "../src/Chromosome.hpp"
#include "../src/common.hpp"

// #include <RandomLib/Random.hpp>


#ifndef USE_BITSET_SEQUENCE

// static void assertPresence(SequenceArray &sequence, MutationSelected &mutation, bool presence)
// {
//   if(presence)
//     ASSERT_EQ(mutation.absPos, sequence.getIfPresent(mutation.absPos)->absPos) ; 
//   else
//     ASSERT_TRUE(sequence.getIfPresent(mutation.absPos) == nullptr); 
// }

// #define ASSERT_PRESENCE(sequence,mutation)  (  )

// TEST(SelectedSequenceTest, fixationTest)
// {
//   Randomness rand(123); 
//   Chromosome chr(100000000,rand);
  
//   SequenceArray seqA(50), seqB(50), seqC(50);
  
//   MutationSelected 
//     mutA(1), mutB(2),  mutC(3);

//   seqA.mutate(&mutA); 
//   seqA.mutate(&mutB); 
//   seqA.mutate(&mutC); 

//   seqB.mutate(&mutA); 
//   seqB.mutate(&mutB); 

//   vector<SequenceArray*> tmp; 
//   tmp.push_back(&seqA); 
//   tmp.push_back(&seqB); 
//   tmp.push_back(&seqC); 

//   chr.reorganizeBitSetMutations(tmp);

//   assertPresence(seqA,mutA, true); 
//   assertPresence(seqA,mutB, true); 
//   assertPresence(seqA,mutC, true); 

//   assertPresence(seqB,mutA, true); 
//   assertPresence(seqB,mutB, true); 
  
//   seqC.mutate(&mutA);

//   chr.reorganizeBitSetMutations(tmp); 

//   assertPresence(seqA,mutA,false);
//   assertPresence(seqB,mutA,false);
//   assertPresence(seqC,mutA,false);

//   seqA.mutate(&mutA); 
//   seqB.mutate(&mutA); 

//   seqB.mutate(&mutC); 
//   seqC.mutate(&mutC); 

//   chr.reorganizeBitSetMutations(tmp); 

//   assertPresence(seqA,mutC,false); 
//   assertPresence(seqB,mutC,false); 
//   assertPresence(seqC,mutC,false); 

//   assertPresence(seqA,mutA,true);
//   assertPresence(seqB,mutA,true);
//   assertPresence(seqC,mutA,false);
  
// }

#endif






