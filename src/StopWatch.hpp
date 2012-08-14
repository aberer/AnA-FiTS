#pragma once 

#include "common.hpp"
#include <sys/time.h>
#include <string>

using namespace std;


// :TODO: multiple calls to watch-elapsed should not add up the time  


class StopWatch
{  
public: 
  StopWatch() : accumulatedTime(0), lastStart(0)
  {
  }
  

  void reset()
  {
    accumulatedTime = 0; 
    lastStart = gettime();
  }
  
  void start()
  {
    lastStart = gettime();
  }
  
  void stop()
  {
    accumulatedTime += gettime() - lastStart;
  }

  double getElapsed()
  {
    stop();
    return accumulatedTime; 
  }


  string getElapsedAsString()
  {
    stop();
    string result = "" ; 
    nat time = round(accumulatedTime); 

    if((time  / 3600) > 1 )      
      {
	result += to_string(round(time / 3600 )) + ":"; 
	time = (time % 3600); 
      }
    else 
      result += "00:" ;
    if((time / 60) > 1)
      {
	result += to_string(round(time / 60)) + ":"; 
	time = (time % 60) ; 
      }
    else 
      result += "00:"; 

    result += to_string(time); 

    return result; 
  }
  


private: 
  double accumulatedTime; 
  double lastStart; 

  double gettime()
  {
    struct timeval ttime;
    gettimeofday(&ttime , 0);
    return ttime.tv_sec + ttime.tv_usec * 0.000001;
  }
}; 
