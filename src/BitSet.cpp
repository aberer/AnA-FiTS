#include "BitSet.hpp"


#define COUNT_WIDTH UCHAR_MAX 



#ifdef HAVE_64BIT
template<> uint64_t BitSet<uint64_t>::allOne = ULONG_MAX; 
template<> uint8_t BitSet<uint64_t>::divOp = 6; 
template<> uint64_t* BitSet<uint64_t>::mask = reinterpret_cast<uint64_t*>(new recMask<uint64_t, 64>()); 
template<> uint64_t* BitSet<uint64_t>::countMask = reinterpret_cast<uint64_t*>(new lookup<uint64_t,COUNT_WIDTH>()); 

template<>
nat BitSet<uint64_t>::countOneElement(uint64_t element) const 
{
  return 
    countMask[ element & 0xffu  ] 
    + countMask[ (element >> 8) & 0xffu  ] 
    + countMask[ (element >> 16) & 0xffu  ]
    + countMask[ (element >> 24) & 0xffu  ]
    + countMask[ (element >> 32) & 0xffu  ]
    + countMask[ (element >> 40) & 0xffu  ]
    + countMask[ (element >> 48) & 0xffu  ]
    + countMask[ (element >> 56) & 0xffu  ]; 
}
#endif


template<> uint32_t BitSet<uint32_t>::allOne = UINT_MAX; 
template<> uint8_t BitSet<uint32_t>::divOp = 5; 
template<> uint32_t* BitSet<uint32_t>::mask = reinterpret_cast<uint32_t*>(new recMask<uint32_t, 32>()); 
template<> uint32_t* BitSet<uint32_t>::countMask = reinterpret_cast<uint32_t*>(new lookup<uint32_t,COUNT_WIDTH>());
template<>
nat BitSet<uint32_t>::countOneElement(uint32_t element) const
{
  return 
    countMask[ element & 0xff  ] 
    + countMask[ (element >> 8) & 0xff  ] 
    + countMask[ (element >> 16) & 0xff  ]
    + countMask[ (element >> 24) & 0xff  ]; 
}



template<> uint16_t BitSet<uint16_t>::allOne = USHRT_MAX; 
template<> uint8_t BitSet<uint16_t>::divOp = 4; 
template<> uint16_t* BitSet<uint16_t>::mask = reinterpret_cast<uint16_t*>(new recMask<uint16_t, 16>()); 
template<> uint16_t* BitSet<uint16_t>::countMask = reinterpret_cast<uint16_t*>(new lookup<uint16_t,COUNT_WIDTH>());
template<> nat BitSet<uint16_t>::countOneElement(uint16_t element) const
{
  return 
    countMask[ element & 0xff  ] 
    + countMask[ (element >> 8) & 0xff  ] ; 
}


template<> uint8_t BitSet<uint8_t>::allOne = UCHAR_MAX; 
template<> uint8_t BitSet<uint8_t>::divOp = 3; 
template<> uint8_t* BitSet<uint8_t>::mask = reinterpret_cast<uint8_t*>(new recMask<uint8_t, 8>()); 
template<> uint8_t* BitSet<uint8_t>::countMask = reinterpret_cast<uint8_t*>(new lookup<uint8_t,COUNT_WIDTH>());
template<>  nat BitSet<uint8_t>::countOneElement(uint8_t element) const
{
  return countMask[ element & 0xff  ] ; 
}
