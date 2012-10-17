#pragma once
#include "common.hpp"
#include "types.hpp"

#include <climits>
#include <iostream>
#include <cstring>
#include <cassert>


// the bit set class itself 
template<typename T>
class BitSet
{
public:   
  static uint64_t convertToInternalSize(uint64_t size); 
  
  explicit BitSet(nat numElems);
  ~BitSet();

  BitSet& operator=(const BitSet& rhs ); 

  void flip();
  void flip(nat pos) ;  
  bool test(nat pos)  const ; 

  void set();
  void set(nat pos); 

  void andify(const BitSet &rhs); 
  void orify( const BitSet &rhs); 

  void orifyPart(const BitSet &rhs, nat startIdx, nat endIdx);

  void unset(nat pos) ; 
  void unset();

  void reset(); 

  uint64_t internalSize() const ;
  uint64_t size() const ; 

  T getPart (nat num) const { return bv[num]; }

  void copyRange( BitSet &donor, nat begin, nat end );

  nat count() const;
  void resize(uint64_t size);

  friend ostream& operator<<(ostream &stream, const BitSet &rhs)    
  {   
    for(nat i = 0; i < rhs.size(); ++i)    
      stream << (rhs.test(i) ? 1 : 0 ) ; 
    return stream; 
  }

  void printRaw(FILE *fh) const;

protected: 
  T *bv;
  uint64_t length; 
  uint64_t numElems; 

private: 
  static const nat maskLength = sizeof(T) * CHAR_BIT; 
  static const nat maxElem = maskLength - 1;

  static T allOne; 
  static uint8_t divOp;

  static T* mask;
  static T* countMask; 

  void copyToEnd(T *donor, T *result, nat start); 
  void copyFromStart(T *donor, T *result, nat end); 
  void copyFromToHelper(T *donor, T *result, nat start, nat end); 
  nat countOneElement(T element) const; 
  BitSet(const BitSet &rhs); 
};



////////////////////
// IMPLEMENTATION //
////////////////////



template<typename T> BitSet<T>::BitSet(nat _numElems)
  :  numElems(_numElems)
{ 
  this->length =  convertToInternalSize(numElems); 
  bv = (T*) calloc(length, sizeof(T));
}


// :KLUDGE:  memory management still not perfect, chromosome breaks otherwise  
template<typename T> BitSet<T>::~BitSet() 
{
  free(bv); 
}

template<typename T> uint64_t BitSet<T>::convertToInternalSize(uint64_t size)
{
  return (size / maskLength) + ((size & maxElem) ?  1 : 0); 
}


template<typename T> void BitSet<T>::set()
{  
  if(numElems == 0)
    return; 

  nat numFullInts = (convertToInternalSize(numElems) - 1 ); 
  memset(bv, UCHAR_MAX, numFullInts  * sizeof(T));   
  for(nat i = numFullInts * sizeof(T) * 8; i < numElems; ++i)
    set(i); 
}


template<typename T> void BitSet<T>::flip()
{
  if(numElems == 0 )
    return; 

  nat numFullInts = (convertToInternalSize(numElems) - 1 ); 
  for(nat i = 0; i < numFullInts; ++i)
    bv[i] = ~bv[i]; 

  for(nat i = numFullInts * sizeof(T) * 8; i < numElems; ++i)
    flip(i); 
}


template<typename T> void BitSet<T>::unset()
{
  if(numElems == 0)
    return; 

  this->bv = (T*)memset(bv, 0, length * sizeof(T));
}


template<typename T> BitSet<T>::BitSet(const BitSet & rhs) 
  : length(rhs.length), numElems(rhs.numElems)
{
  bv = (T*)malloc(rhs.length * sizeof(T));
  copy(rhs.bv , rhs.bv+length, this->bv);  
}


template<typename T> void BitSet<T>::reset()
{
  if(numElems == 0)
    return; 
  this->bv = (T*)memset(bv, 0, length * sizeof(T));
}



template<typename T> inline void BitSet<T>::copyFromToHelper(T *donor, T *result, nat start, nat end)
{
  assert(end < sizeof(T) * 8); 
  *result |= *(donor) & ((allOne << start) &  (allOne >> (maskLength-end)));
}

template<typename T> inline void BitSet<T>::copyFromStart(T *donor, T *result, nat end)
{
  assert(end < sizeof(T) * 8); 
  *result |= *(donor) & (allOne >> (maskLength-end));
}


template<typename T> inline void BitSet<T>::copyToEnd(T *donor, T *result, nat start)
{
  assert(start < maskLength); 
  *result |= *(donor) & (allOne << start);
}

// :TRICKY: assuming, that the bitstring is nulled  
template<typename T> void BitSet<T>::copyRange( BitSet &donor, nat begin, nat end )  
{
  nat startInt = begin >> divOp ; 
  nat endInt = end >> divOp;

  begin = begin & maxElem; 
  end = end & maxElem; 
  
  T* donorBv = donor.bv + startInt; 
  T* recBv = bv + startInt; 
  int bytesToCopy = endInt - startInt - 1 ; 
  
  if(startInt == endInt)
    {
      copyFromToHelper(donorBv, recBv, begin, end); 
    }
  else
    {
      copyToEnd(donorBv, recBv,begin); 
      ++donorBv; 
      ++recBv; 
      memcpy(recBv , donorBv, bytesToCopy * sizeof(T)); 
      donorBv += bytesToCopy; 
      recBv += bytesToCopy; 
      copyFromStart(donorBv, recBv,end); 
    }
}


template<typename T> nat BitSet<T>::count() const 
{
  nat result = 0;   
  for(nat i = 0; i < length; ++i)
    result += countOneElement(bv[i]) ;
  return result; 
}


template<typename T> void BitSet<T>::resize(uint64_t size)
{
  nat newInternalSize = BitSet::convertToInternalSize(size);   
  if(newInternalSize != length)
    {
      this->bv = (T*)realloc(this->bv, newInternalSize *  sizeof(T)); 
      memset(bv, 0 , newInternalSize * sizeof(T)); 
      this->length = newInternalSize; 
      this->numElems = size; 
    }
  else
    {
      this->numElems = size; 
      memset(this->bv, 0, sizeof(T) * length);
    }  
}


template<typename T> BitSet<T>& BitSet<T>::operator=(const BitSet<T>& rhs )
{  
  if(&rhs == this)
    return *this; 

  length = rhs.length; 
  numElems = rhs.numElems;   

  bv = (T*)realloc(bv, rhs.length * sizeof(T)); 
  copy(rhs.bv , rhs.bv+length, this->bv);  
  
  return *this;
}


template<typename T> void BitSet<T>::set(nat pos)
{ 
  assert(pos < numElems) ; 
  bv[ pos >> divOp ] |= mask[pos & maxElem];
}  


template<typename T> bool BitSet<T>::test(nat pos)  const 
{ 
  assert(pos < numElems) ; 
  return (bv[pos >> divOp ] & mask[pos & maxElem]); 
}


template<typename T> void BitSet<T>::flip(nat pos)  
{ 
  assert(pos < numElems) ; 
  bv[ pos  >> divOp ] ^= mask[pos  & maxElem ] ;
}


template<typename T> void BitSet<T>::unset(nat pos)  
{
  assert(pos < numElems) ; 
  bv[ pos >> divOp] &= (~ mask[pos & maxElem]) ;
}

template<typename T> uint64_t BitSet<T>::internalSize() const
{
  return length;
}

template<typename T> uint64_t BitSet<T>::size() const 
{
  return numElems; 
}


template<typename T > void BitSet<T>::andify(const BitSet &rhs)
{
  assert(rhs.numElems == this->numElems); 
  for(nat i = 0 ; i < length; ++i)
    this->bv[i] &= rhs.bv[i]; 
}


/** 
    orify sub-bitset including first and excluding last index 
 */ 
template<typename T> void BitSet<T>::orifyPart(const BitSet &rhs, nat begin, nat end)
{  
  if(begin == end)
    return;   
  
  nat startInt = begin >> divOp ; 
  nat endInt = end >> divOp;

  begin = begin & maxElem; 
  end = end & maxElem; 

  T* donorBv = rhs.bv + startInt; 
  T* recvBv = bv + startInt; 
  int bytesToCopy = endInt - startInt - 1; 

  if(startInt == endInt)
    copyFromToHelper(donorBv, recvBv, begin, end); 
  else 
    {      
      copyToEnd(donorBv, recvBv, begin); 
      ++donorBv; 
      ++recvBv; 

      for(int i = 0; i < bytesToCopy ; ++i)
	{
	  *recvBv |= *donorBv; 
	  ++donorBv; 
	  ++recvBv; 
	} 
      
      if(end)
	copyFromStart(donorBv, recvBv,end); 
    }
}

 

template<typename T> void BitSet<T>::orify(const BitSet &rhs)
{
  assert(rhs.numElems == this->numElems); 
  for(nat i = 0 ; i < length; ++i)
    this->bv[i] |= rhs.bv[i]; 
}


// the bit mask for comparison 
template <typename T, nat  I> struct recMask 
{
  recMask<T,I - 1> rest;
  T RES;
  recMask() : RES(1ul << (I-1)) {} 
};

template <typename T> struct recMask<T, 1> 
{
  T RES;
  recMask() : RES(1) {}  
};


// bit counting 
template<nat N> struct BitCount{ enum { RES = ( N & 1 ? 1 : 0 )  + BitCount< (N>>1) >::RES   }; }; 
template<> struct BitCount<0> { enum {  RES = 0};  }; 


// the lookup table 
template<typename T, nat I > struct lookup
{
  lookup<T,I-1> rest;
  T RES; 
  lookup() : RES(BitCount<I>::RES) {} 
}; 


template<typename T> struct lookup<T,0> 
{  
  T RES;    
  lookup() : RES(0) {} 
}; 



template<typename T> void BitSet<T>::printRaw(FILE *fh) const
{
  for(nat i = 0; i < length; ++i)
    BIN_WRITE(bv[i], fh); 
}
