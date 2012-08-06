

template<typename TYPE> void Randomness::IntegerArray(TYPE *array, nat length, nat upperBound)
{
  constexpr nat byteInType = sizeof(TYPE); 
  uint32_t* startEfficient = reinterpret_cast<uint32_t*>(array); 
  nat divisor = getR2<TYPE>(upperBound);  
  size_t numBytes = byteInType * length; 
  
  initArray(reinterpret_cast<uint8_t*>(startEfficient), numBytes); 
  nat iterations = numBytes / sizeof(uint32_t); 
  constexpr nat numReps = sizeof(uint32_t) / sizeof(TYPE); 
  
  for(nat i = 0; i < iterations; ++i)
    {
      pun_t<uint32_t, TYPE> converter; 
      converter.whole = startEfficient[i];

      converter.whole ^= converter.whole >> 11;
      converter.whole ^= converter.whole <<  7 & 0x9d2c5680UL;
      converter.whole ^= converter.whole << 15 & 0xefc60000UL;
      converter.whole ^= converter.whole >> 18;
      
      for(nat j = 0; j < numReps; ++j )
	converter.part[j] /= divisor; 
      startEfficient[i] = converter.whole; 
    }
}
