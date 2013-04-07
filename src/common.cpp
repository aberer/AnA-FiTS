#include "common.hpp"
#include "types.hpp"

#include <cstdlib>
#include <cstring>

char *binary (unsigned int v) {
  static char binstr[17] ;
  int i ;

  binstr[16] = '\0' ;
  for (i=0; i<16; i++) {
    binstr[15-i] = v & 1 ? '1' : '0' ;
    v = v / 2 ;
  }

  return binstr ;
}


template<>
void intNormalizer<uint16_t>(uint16_t number,unsigned int &m, unsigned int &r2); 




string getBinary(int tmp, nat numBits) 
{
  string result = "";   
  for(nat i = 0; i < numBits; ++i)
    result =  (((tmp >> i) & 1  ) ?  "1" : "0"  ) + result;
  return result; 
}


#include <cassert>

void* malloc_aligned(size_t size, size_t align)
{

  void *ptr = (void *)NULL;  
  int res;

#if defined(__APPLE__)
  ptr = malloc(size); 
  memset(ptr, 0, sizeof(size));   

  if(ptr == NULL)
    assert(0); 
#else 

  res = posix_memalign( &ptr, align, size );
  memset(ptr, 0, size); 

  if(res != 0) 
    assert(0); 

#endif

  return ptr;
}


#include <iostream>

void* myRealloc(void *ptr, size_t num)
{
  void *newPtr; 
  newPtr = realloc(ptr, num); 
  if(NOT newPtr)
    {
      free(ptr); 
      std::cerr << "could not reallocate array. Aborting." << endl; 
      abort();
    }
  
  return newPtr; 
}


#include <sys/stat.h>


#ifdef PRODUCTIVE
static bool fileExists(const std::string& filename)
{
  struct stat buf;
  return stat(filename.c_str(), &buf) != -1; 
}
#endif

FILE* openFile(string name, string mode)
{
#ifdef PRODUCTIVE
  if(fileExists(name))
    {
      cerr << "A file with the name >" << name << "< already existis. Please choose a different id." << endl; 
      abort(); 
      return nullptr; 
    }
#endif

  FILE *fp = fopen(name.c_str(), mode.c_str());   
  return fp; 
}
