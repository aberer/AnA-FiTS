#pragma once 
#include "common.hpp"
#include "DynArray.hpp"
#include "BitSet.hpp"
#include "TimeSection.hpp"


// :TRICKY: and extremely important. All generation indices are
// absolute indices. However, the arrays only contain a small portion
// of that, since we are only simulating a small window

class Survivors : private TimeSection
{
public: 
  Survivors(nat popSize, nat maxPopSize, nat startGen, nat endGen);
  ~Survivors();

  void prepareNextGenBackwards(); 
  nat* getFirstSurvivorBackwards(nat fromLastGeneration) const ; 
  nat* getFirstSurvivorForward(nat genNum) const ;  
  void setFirstSuvivorRelativeToNow(nat fromLastGeneration, nat *firstSurvivor) ; 
  void addSurvivor(nat idx); 
  nat* getLastSurvivor() const;
  void printGen(nat gen) const; 
  void reserve(nat num);

private:
  nat *survivors; 
  nat capacity;
  nat *nextSurvivor;  		// points to the next free position in nextSurvivor  
  nat *survivorAccCountByGen; 
  BitSet<uint32_t> setMaker;  
  nat highestIndex; 
  nat currentGenerationBackwards ; // starting with 0, ending at maximum generation (which is actually the start)

  void resize();
  void initializePresentGeneration(nat howMany);

  friend class SurvivorsTest; 

  friend ostream& operator<<(ostream &stream, const Survivors &rhs); 
};



inline ostream&  operator<<(ostream &stream, const Survivors &rhs)
{
  for(nat i = 0; i < rhs.highestIndex+1; ++i)
    {
      nat *start = rhs.survivors + rhs.survivorAccCountByGen[i]; 
      nat *end = rhs.survivors + rhs.survivorAccCountByGen[i+1]; 
      
      stream << "gen " << rhs.highestIndex-i << ": "; 
      for(nat *iter = start; iter < end; ++iter )
	cout << *iter << ","; 
      cout << endl;
    }
  return stream; 
}


inline void Survivors::printGen(nat gen) const
{
  nat *start = survivors + survivorAccCountByGen[gen]; 
  nat *end = survivors + survivorAccCountByGen[gen+1]; 

  cout << "gen " << gen ; 
  for(nat *iter = start ; iter < end ; ++iter)
    cout << *iter << "," ; 
  cout << endl; 
}


// :TRICKY: no correction necessary? 
inline nat* Survivors::getFirstSurvivorBackwards(nat fromLastGeneration) const
{    
  return  survivors + survivorAccCountByGen[fromLastGeneration] ;
}  // different: gen 0 is present gen   



inline nat* Survivors::getFirstSurvivorForward(nat genNum) const
{ 
  return survivors + survivorAccCountByGen[highestIndex-this->getGenIdx(genNum)] ;
}    // usual numbering gen 0 was first gen in past  


inline void Survivors::setFirstSuvivorRelativeToNow(nat fromLastGeneration, nat *firstSurvivor) 
{   
  survivorAccCountByGen[fromLastGeneration] = firstSurvivor - survivors;
}


inline void Survivors::prepareNextGenBackwards()
{
  // cout << highestIndex - currentGenerationBackwards  << "\t" << setMaker.count() << endl;
  currentGenerationBackwards++; 
  survivorAccCountByGen[currentGenerationBackwards] = nextSurvivor - survivors;   
  setMaker.reset();
}

inline nat* Survivors::getLastSurvivor() const {return nextSurvivor ; } 

inline void Survivors::addSurvivor(nat idx)
{ 
  if(NOT setMaker.test(idx))
    {
      *nextSurvivor = idx;
      nextSurvivor++;
      setMaker.set(idx);
    }
}


inline void Survivors::reserve(nat num)
{
  nat numUsed = nextSurvivor - survivors; 
  // cout  << "RESERVE " << numUsed <<  " / " << capacity << " used" << endl;
  while( capacity <  numUsed + num ) 
    resize();
}
