#include "ChromConfig.hpp"

// template<> const uint8_t ChromConfig<2>::mask = 1; 
// template<> const uint8_t ChromConfig<4>::mask = 3; 

template<> 
const uint8_t ChromConfig<2>::mask[8] =   
  {
    1, 2, 4, 8, 16, 32, 64, 128
  };


template<>
const uint8_t ChromConfig<2>::doubleMask[8] = 
  {
    3, 6, 12, 24, 48, 96, 192, 0
    
  }; 
