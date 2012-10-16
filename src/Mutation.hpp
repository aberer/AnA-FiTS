#pragma once 

#include "types.hpp"
#include "common.hpp"

// #include "Node.hpp"
#include "Base.hpp"


#include <fstream>


// INSTANCES 
struct SelectedMutation
{
  seqLen_t absPos;
  nat generation;
  Base base;  
  FITNESS_TYPE fitness;
  nat indiNr;
  bool inUse; 

  bool isClaimed() {return inUse; }
  void claim() { inUse = true; }
  void unclaim() { inUse = false; }
  void printRaw(FILE *fh); 
  SelectedMutation(){};
  SelectedMutation(const SelectedMutation &rhs)
    : absPos(rhs.absPos)
    , generation(rhs.generation)
    , base(rhs.base)
    , fitness(rhs.fitness)
    , indiNr(rhs.indiNr) 
  { }

};

struct NeutralMutation
{
  seqLen_t absPos;
  nat generation;
  Base base;  
  
  bool inUse; 
  bool isClaimed() {return inUse; }
  void claim() {inUse = true; }
  void unclaim() {inUse = false; }
  void printRaw(FILE *fh);
 };


ostream& operator<<(ostream &stream, const Base base); 
ostream& operator<<(ostream &stream, const NeutralMutation &rhs); 
ostream& operator<<(ostream &stream, const SelectedMutation &rhs); 

