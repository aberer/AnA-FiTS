#pragma once

#include "JudyArray.hpp"
#include "common.hpp"
#include "types.hpp"

#include <string>
#include <vector>


class ProgramOptions; 
class Event; 

class PopulationParams
{
public: 
  PopulationParams(nat id, nat numberOfGenerations, popSize_t initSize, const ProgramOptions &progOpt);

  // for TESTING 
  PopulationParams(popSize_t initSize, double recRate, double mutRate);  

  nat getPopSizeByGeneration(const nat genNum)  const ; 

  // :TODO: currently gen independant  
  double getRecombinationRate(nat gen ) const {return recombinationRate; }
  // double getGeneConverBsionRate(nat gen ) const {return geneConversionRate; }
  double getMutationRate( nat gen ) const  {  return mutationRate ; ;} // is neutral ? 

  void parseEvents(vector<string> stringEvents, nat id);

private:      
  JudyArray<judyKey,Event*> events; 
  popSize_t initSize; 
  nat correction; 
  
  // TODO make time dependent as well 
  double recombinationRate ; 
  // double geneConversionRate; 
  double mutationRate; 

  // :TODO: neutrality 
  bool isNeutral; 
};



