#pragma once

#include "config.hpp"

#include <string>
#include <limits>


#define APPEND(elem, list) ((elem)->next = (list) , (list) = (elem))

#define POWER_2(x) (1 << (x))


#define DIVIDE_2(x) (x >> 1)
#define DIVIDE_4(x) (x >> 2)
#define DIVIDE_8(x) (x >> 3)
#define DIVIDE_16(x) (x >> 4)
#define DIVIDE_32(x) (x >> 5)
#define DIVIDE_64(x) (x >> 6)

#define MULT_2(x) (x << 1)
#define MULT_4(x) (x << 2)
#define MULT_8(x) (x << 3)
#define MULT_16(x) (x << 4)
#define MULT_32(x) (x << 5)
#define MULT_64(x) (x << 6)

#define MODULO_8(x) (x & 7) 
#define MODULO_16(x) (x & 15) 
#define MODULO_32(x) (x & 31) 
#define MODULO_64(x) (x & 63)

#define ALIGN(num)   __attribute__ ((aligned (num))); 



using namespace std; 

#define NOT ! 
// #define NOT_EQUAL_0(x) ( NOT x )
#define EQUAL_1(x) (x & 1)
#define SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

// if not there 
#define nullptr 0

// **************** passive structs 


char *binary (unsigned int v); 


template<typename T>
void intNormalizer(T number,unsigned int &m, unsigned int &r2)
{
  number--; 
  number = number ? number : 1U; 
  // :TRICKY: this case shoudl never occur in our case 
  // T  n1 = ~number ? number + 1U : 1U;    
  T n1 = number + 1U; 
  T r1 =  (numeric_limits<T>::max() - number) / n1; 
  m = r1 * n1  + number; 
  r2 = r1 + 1U;     
}

// little helpers 
string getBinary(int tmp, unsigned int numBits); 

#define CHOOSE_IMPLEMENTATION_BY_TYPE(type, function, ...)		\
  if(type <= std::numeric_limits<uint8_t>::max())			\
    {									\
      function<uint8_t>(__VA_ARGS__);					\
    }									\
  else if(type <= std::numeric_limits<uint16_t>::max())			\
    {									\
      function<uint16_t>(__VA_ARGS__);					\
    }									\
  else if(type <= std::numeric_limits<uint32_t>::max())			\
    {									\
      function<uint32_t>(__VA_ARGS__);					\
    }									\
  else									\
    assert(0);								


// only relevant for resampleParentsByFitness
#define CHOOSE_IMPLEMENTATION_BY_TYPE_AND_NEUTRALITY(type, neutral, function, ...) \
  if(type <= std::numeric_limits<uint8_t>::max())			\
    function<uint8_t,neutral>(__VA_ARGS__);					\
  else if(type <= std::numeric_limits<uint16_t>::max())			\
    function<uint16_t,neutral>(__VA_ARGS__);					\
  else if(type <= std::numeric_limits<uint32_t>::max())			\
    function<uint32_t,neutral>(__VA_ARGS__);					\
  else									\
    assert(0);								


void* malloc_aligned(size_t size, size_t align); 

#define ADD_PADDING_16(x) ((MODULO_16(x)) ? (16 - (MODULO_16(x))) : 0 )

void* myRealloc(void *ptr, size_t num); 

#define GET_OTHER_ANCESTOR(x) ((x & 1) ? x-1 : x+1)

#define NO_NODE 0

// #define IS_ODD(x) (x&1)


#define BIN_WRITE(x,fh) fwrite(&x, 1, sizeof(x), fh)
#define BIN_READ(x,fh) fread(&x, 1, sizeof(x), fh)

#define NODE_IS_RELEVANT(x,start,end) (start <= x && x <= end)


FILE* openFile(string name, string mode); 

#define SEQ_FILE_NAME "anafits_polymorphisms"
#define ARG_FILE_NAME "anafits_graph"
#define INF_FILE_NAME "anafits_info"

#define INIT_FITNESS  1.0

#define NUMBER_PARENTS 2
#define VERSION 0.9


// this is a bad hack 
#define INVERT_INT(x) (x + std::numeric_limits<int>::min())
#define REVERT_INT(x) (x - std::numeric_limits<int>::min())
#define IS_INVERTED(x) (x < 0)
