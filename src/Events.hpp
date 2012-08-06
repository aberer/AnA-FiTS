#include "common.hpp"

class Event
{
protected:   
  double sizeAtBegin; 
  nat when; 
  nat correction;
public:
  virtual nat getByTime(nat time) = 0; 
  virtual void init(nat lastSize) = 0 ; 
  nat getWhen(){return when; }
}; 


class DiscreteChange : public  Event
{  
public: 
  float rate;  
  DiscreteChange(nat when, float rate, nat correction ) : rate(rate) 
  { 
    this->when = when ; 
    this->correction = correction;  
  }

  virtual void init(nat lastSize){ sizeAtBegin = (nat(lastSize * rate) | correction) - correction  ; }  
  virtual nat getByTime(nat time) {return  sizeAtBegin;  }
}; 


class LogisticGrowth : public Event
{
public: 
  float rate; 
  nat capacity; 
  LogisticGrowth(nat when, float rate, nat capacity, nat correction ) :  rate(rate), capacity(capacity) { this->when = when; this->correction  = correction; } 

  virtual void init(nat lastSize) {  sizeAtBegin = lastSize;  }
  virtual nat getByTime(nat time)
  {  
    nat timePassed = (time - this->when);     
    float  result = (((capacity / sizeAtBegin) -  1 ) * exp( -1. * rate * timePassed ))  + 1  ; 
    result = capacity / result; 
    nat intResult  =   (nat(result) | correction) - correction; 
    return intResult;    
  }
}; 


class ExponentialGrowth : public Event
{  
public: 
  float rate ; 
  ExponentialGrowth(nat when, float rate, nat correction) : rate(rate){this->when = when; this->correction = correction;  }
  virtual void init(nat lastTime) { sizeAtBegin = lastTime; }
  virtual nat getByTime(nat time) 
  {
    nat timePassed = (time - when ); 
    return (nat (exp(rate * timePassed ) * sizeAtBegin) | correction ) - correction ; 
  }
}; 

class ExponentialDecay : public Event
{  
 public: 
 float rate ; 
 ExponentialDecay(nat when, float rate, nat correction) : rate(rate){this->when = when; this->correction = correction;  }
  virtual void init(nat lastTime) { sizeAtBegin = lastTime; }
  virtual nat getByTime(nat time) 
  {
    nat timePassed = (time - when ); 
    return (nat (exp(-1.  * rate * timePassed ) * sizeAtBegin) | correction ) - correction ; 
  }
}; 
