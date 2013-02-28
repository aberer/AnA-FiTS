#include "config.h"

#ifdef ARE_PRODUCTIVE
#define PRODUCTIVE
#endif


///////////////////
// OPTIMIZATION  //
///////////////////

// #define _TEST

#define CONSOLE_OUTPUT
// #define PRINT_SEQ_STDOUT

#define FITNESS_TYPE double


// #define DEBUG_ALT_CLEANUP 

// :TRICKY: this works, but on default calls will not yield any
// improvement. It is good, if the analysis is broken down into steps
// because of memory, then the graph will be reduced.  #define
// FEATURE_REDUCE_GRAPH


// #define DEBUG_POP_PARAM_PARSE  

// expensive 
// #define  DEBUG_CHECK_ARRAY_CONSISTENCY

///////////////////
// DEBUG_FORWARD //
///////////////////

// #define DEBUG_RECOMBINATION
// #define DEBUG_MUT_SEL
// #define DEBUG_PRINT_POP_EVERY_GEN

// #define DEBUG_SHOW_POPSIZE
// #define DEBUG_SHOW_GENS

// #define DEBUG_PRECOMPUTE 
// #define DEBUG_RECMAN
// #define DEBUG_REPORT_FIXATIONS
// #define DEBUG_HAPLO_WINDOW 
// #define DEBUG_FITNESS
// #define DEBUG_FITNESS_FUNC
// #define DEBUG_SURVIVORS  

///////////////
// BACKWARDS //
///////////////

// #define DEBUG_GET_COAL_STATISTIC
// #define DEBUG_UPDATE_GRAPH
// #define DEBUG_HOOKUP 
// #define DEBUG_BACKTRACE 
// #define DEBUG_SEQUENCE_EXTRACTION
