#pragma once 

#include "types.hpp"
#include "common.hpp"

#include <fstream>

enum Base : uint8_t    { A = 0, C = 1 , G = 2 , T = 3 }; 

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

