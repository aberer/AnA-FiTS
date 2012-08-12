#include "PopulationParams.hpp"
#include "ProgramOptions.hpp"
#include "Events.hpp"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

/**
   Purpose of this class: provide parameters at all times for a
   specific population.   
 */ 

// TODO currently we are not safe against int overflow, when the user wans to simulate crazily large pop sizes

void formatComplainEnd()
{
  cerr << "\n\nPlease mind the format for timed events or rates\n\n" << endl; 
  abort();
}

void saveProceed(vector<string>::iterator &iter, vector<string>::iterator end)  
{
  iter++; 
  if(iter == end)
    formatComplainEnd();
}


float parseRate(vector<string> args, nat id )
{
  float result = 0; 
  
  auto i = args.begin();
  bool isFirst = true; 
  while(i !=  args.end())
    {
      if(isFirst &&  i->compare("P") )	
	{
	  result = atof(i->c_str()); 	  
	}
      else
	{
	  isFirst = false; 
	  saveProceed(i, args.end()); 
	  nat idHere = atoi(i->c_str()); 
	  saveProceed(i,args.end()); 
	  if(idHere == id)
	    result = atof(i->c_str()); 
	}
      isFirst = false; 
      i++;
    }
  
  return result; 
}


PopulationParams::PopulationParams(nat id, nat numberOfGenerations, popSize_t initSize, const ProgramOptions &progOpt)  
  : initSize(initSize)  
  , correction(1)
  // , correction(progOpt.get<nat>("ploidy") - 1 )
  , isNeutral(false)
{
  vector<string> stringEvents; 
  if(progOpt.hasOption("popEvent"))
    stringEvents = progOpt.get<vector<string> >("popEvent");

  // parse rates 
  recombinationRate = parseRate(progOpt.get<vector<string> >("recRate"),id);
  // geneConversionRate = parseRate(progOpt.get<vector<string> >("mutRate"), id);
  mutationRate = parseRate(progOpt.get<vector<string> >("mutRate"), id);  

  if(NOT stringEvents.empty())
    {
      parseEvents(stringEvents,id);
      nat lastPopSize = initSize; 

      // for(auto event : events)
      for(auto eventI = events.begin(); eventI  !=  events.end(); ++eventI)
	{
	  (*eventI)->init(lastPopSize); 

	  if(eventI+1 != events.end())
	    {
	      nat whenIsNext = (*(eventI+1))->getWhen(); 
	      lastPopSize = (*eventI)->getByTime(whenIsNext-1); 
	    }
	}
    }
}


void PopulationParams::parseEvents(vector<string> stringEvents, nat id) 
{  
  auto iter = stringEvents.begin(); 
  auto theEnd = stringEvents.end();
  while( iter != theEnd )
    { 
      char type = iter->at(0);
      saveProceed(iter, theEnd); 

      nat theTime = atoi(iter->c_str()) ; 
      iter++; 

      if( iter != theEnd &&  NOT iter->compare("P") )
	{
	  saveProceed(iter, theEnd);
	  nat idOfEvent = atoi(iter->c_str()); 

	  // continue, this is not for us 
	  if(id != idOfEvent)
	    { 
	      while(  iter !=  theEnd &&  (iter->compare("c") && iter->compare("d")  && iter->compare("g") && iter->compare("k")))
		iter++;	      
	      continue;
	    }
	  else
	    saveProceed(iter, theEnd);
	}

      if(iter == theEnd)
	formatComplainEnd();

      Event *event;
      switch(type)
	{
	case 'c':
	  {
	    float theRate = atof(iter->c_str()); 
	    if(theRate == 0.0f)
	      {
		cerr << "population event rate could not be parsed (or was zero)" << endl; 
		abort(); 
	      }
	    
	    iter++; 
	    event = new DiscreteChange(theTime, theRate, correction); 
	    break;
	  } 
	case 'd': 
	  {
	    float rate  = atof(iter->c_str()) ; 
	    if(rate == 0.0f)
	      {
		cerr << "population event rate could not be parsed (or was zero)" << endl; 
		abort(); 
	      }
	    iter++; 
	    event = new ExponentialDecay(theTime, rate, correction) ; 
	    break; 
	  } 
	case 'g': 
	  {
	    float rate  = atof(iter->c_str()) ; 
	    if(rate == 0.0f)
	      {
		cerr << "population event rate could not be parsed (or was zero)" << endl; 
		abort(); 
	      }
	    iter++; 
	    event = new ExponentialGrowth(theTime, rate, correction) ; 
	    break; 
	  } 
	case  'k':
	  {
	    nat capacity = atoi(iter->c_str()); 
	    if(capacity == 0)
	      {
		cerr << "population event rate could not be parsed (or was zero)" << endl; 
		abort(); 
	      }
	    saveProceed(iter, theEnd);
	    float rate = atof(iter->c_str()); 
	    if(rate == 0.0f)
	      {
		cerr << "population event rate could not be parsed (or was zero)" << endl; 
		abort(); 
	      }
	    iter++; 
	    event = new LogisticGrowth(theTime, rate, capacity, correction); 
	    break;
	  }
	default: 
	  formatComplainEnd();
	}      

      // finally insert the event 
      bool wasInserted = false; 
      for(auto eventI = events.begin(); eventI < events.end(); ++eventI)
	{
	  if(event->getWhen() < (*eventI)->getWhen())
	    {
	      events.insert(eventI, event); 
	      wasInserted = true; 
	      break; 
	    }
	}
      if(NOT wasInserted)
	events.push_back(event);
    }
}


nat PopulationParams::getPopSizeByGeneration(const nat genNum)   const // 
{
  auto end = events.end(); 
  for(auto eventI = events.begin(); eventI != end; ++eventI )
    if((*eventI)->getWhen() <= genNum 
       && (eventI+1 == end ||  genNum < (*(eventI+1))->getWhen() ))
      return (*eventI)->getByTime(genNum);
  
  return initSize; 
}


PopulationParams::PopulationParams(popSize_t _initSize, double _recRate, double _mutRate)
  : initSize(_initSize)
  , recombinationRate(_recRate)
  , mutationRate(_mutRate)
{
}
