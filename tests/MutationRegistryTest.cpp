// #include <gtest/gtest.h> 
// // #include "../src/MutationRegistry.hpp"
// #include "../src/common.hpp"
// #include "../src/Chromosome.hpp"


// #define LENGTH 1000000


// TEST(MutationRegistryTest, defaultTest)
// {
//   function<float(Randomness&)> fitnessModell = [&](Randomness &rng){return rng.Float(); }; 
  
//   Chromosome chr(LENGTH, fitnessModell); 
//   MutationRegistry reg(fitnessModell, LENGTH); 
//   Randomness rng(123) ; 

//   SelectedMutation mut; 
//   reg.initSelectedMutation(rng, mut, chr.getSeqRep()); 
  
// }
