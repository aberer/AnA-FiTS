#pragma once 

#include <cassert>
#include <xmmintrin.h>

#include "../lib/asmlib.h"


// this is an interrupt 
// __asm__ { "int $0x03"; } 

#define SHIFT_LEFT_32(x,y)  (_mm_slli_epi32(x, y))
#define SHIFT_RIGHT_32(x,y)  (_mm_srli_epi32(x, y))
#define OR(x,y) ( _mm_or_si128(x,y))
#define AND(x,y) ( _mm_and_si128(x,y))
#define ANDNOT(x,y) (_mm_andnot_si128(x,y))
#define XOR(x,y) (_mm_xor_si128(x,y))


template<typename TYPE>__m128i setVariable(TYPE var); 
template<> inline __m128i setVariable(uint16_t var) {  return _mm_set1_epi16(var); }
template<> inline __m128i setVariable(uint8_t var)  {  return _mm_set1_epi8(var); }
template<> inline __m128i setVariable(uint32_t var) {  return _mm_set1_epi32(var); }

template<typename TYPE> void setDivisor(__m128i buf[2], TYPE d); 
template<> inline void setDivisor(__m128i buf[2], uint32_t d) {  setdivisorV4u32(buf, d); }
template<> inline void setDivisor(__m128i buf[2], uint16_t d) {  setdivisorV8u16( buf,  d); }
template<> inline void setDivisor(__m128i buf[2], uint8_t d) {  assert(0);  }


template<typename TYPE> __m128i shiftLeft(__m128i a, uint32_t shift); 
template<> inline __m128i shiftLeft<uint8_t>(__m128i a, uint32_t shift) { assert(0) ; return a; } 
template<> inline __m128i shiftLeft<uint16_t>(__m128i a, uint32_t shift) { return _mm_slli_epi16(a,shift); } 
template<> inline __m128i shiftLeft<uint32_t>(__m128i a, uint32_t shift) { return _mm_slli_epi32(a,shift); } 

template<typename TYPE> __m128i shiftRight(__m128i a, uint32_t shift); 
template<> inline __m128i shiftRight<uint8_t>(__m128i a, uint32_t shift){ assert(0) ; return a; } 
template<> inline __m128i shiftRight<uint16_t>(__m128i a, uint32_t shift){ return _mm_srli_epi16(a,shift); } 
template<> inline __m128i shiftRight<uint32_t>(__m128i a, uint32_t shift){ return _mm_srli_epi32(a,shift); } 


template<typename TYPE> __m128i divide(const __m128i buf[2], __m128i x); 
template<> inline __m128i divide<uint8_t>(const __m128i buf[2], __m128i x)  {assert(0); return dividefixedV8u16(buf,x);  }
template<> inline __m128i divide<uint16_t>(const __m128i buf[2], __m128i x)  {return dividefixedV8u16(buf,x); }
template<> inline __m128i divide<uint32_t>(const __m128i buf[2], __m128i x)  {return dividefixedV4u32(buf,x); }


template<typename TYPE> __m128i gt(const __m128i a, const __m128i b); 
template<> inline __m128i gt<uint32_t>(const __m128i a, const __m128i b) {return _mm_cmpgt_epi32(a,b); } 
template<> inline __m128i gt<uint16_t>(const __m128i a, const __m128i b) {return  _mm_cmpgt_epi16(a,b); } 
template<> inline __m128i gt<uint8_t>(const __m128i a, const __m128i b) {assert(0); return   _mm_cmpgt_epi8(a,b); } 


template<typename TYPE> __m128i eq(const __m128i a, const __m128i b); 
template<> inline __m128i eq<uint32_t>(const __m128i a, const __m128i b) {return _mm_cmpeq_epi32(a,b); } 
template<> inline __m128i eq<uint16_t>(const __m128i a, const __m128i b) {return  _mm_cmpeq_epi16(a,b); } 
template<> inline __m128i eq<uint8_t>(const __m128i a, const __m128i b) {assert(0); return   _mm_cmpeq_epi8(a,b); } 


template<typename TYPE> __m128i getLSB(__m128i word)
{
  __m128i tmp = setVariable<TYPE>(1); 
  return AND(word,tmp); 
} 


template<typename TYPE>
void print128(__m128i var, string name); 
template<> inline void print128<uint16_t>(__m128i var, string name) 
{
  cout << name  << _mm_extract_epi16 (var, 0) << " " << _mm_extract_epi16 (var, 1)<< " " << _mm_extract_epi16 (var, 2)<< " " << _mm_extract_epi16 (var, 3)<< " " << _mm_extract_epi16 (var, 4)<< " " << _mm_extract_epi16 (var, 5)<< " " << _mm_extract_epi16 (var, 6)<< " " << _mm_extract_epi16 (var, 7) << " " << endl; 
}

// template<> inline void print128<uint32_t>(__m128i var, string name) 
// {
//   cout << name  << _mm_extract_epi32 (var, 0) << " " << _mm_extract_epi32 (var, 1)<< " " << _mm_extract_epi32 (var, 2)<< " " << _mm_extract_epi32 (var, 3)<< " " << endl; 
// }


template<typename TYPE> uint32_t mapMask(uint32_t mask ); 
template<> inline uint32_t mapMask<uint16_t>(uint32_t mask)
{
  return 
    (mask & 1)
    | ((mask & 4) >> 1 ) 	// shift 3 to 2 
    | ((mask & 16 ) >> 2)  	// shift 5 to 3 
    | ((mask & 64)  >> 3)	// ...
    | ((mask & 256) >> 4)
    | ((mask & 1024) >> 5)
    | ((mask & 4096) >> 6)
    | ((mask & 16384) >> 7); 
}

template<> inline uint32_t mapMask<uint32_t>(uint32_t mask)
{
  assert(0); 
  return 
    (mask & 1 )
    | ((mask & 16) >> 3)
    | ((mask & 256) >> 6)
    | ((mask & 4096) >> 9);
}
