#pragma once 


enum Base : uint8_t
  { 
    A = 0, C = 1 , G = 2 , T = 3 
      }; 

static const char* BaseString[] = {"A", "C", "G", "T"} ; 

// char toChar(Base base); 



// char toChar(Base base); 
// {
//   switch(base)
//     {
//     case A:
//       return 'A'; 
//     case C: 
//       return 'C' ; 
//     case G:
//       return 'G';
//     case T:
//       return 'T'; 
//     default : 
//       {
// 	assert(0); 
// 	return '0'; 
//       }
      
//     }
// }
