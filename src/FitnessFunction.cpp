#include "FitnessFunction.hpp"

#include <boost/algorithm/string.hpp>


FitnessFunction::FitnessFunction(vector<string> args)
{
  string modeString =  args[0];   
  if( NOT modeString.compare("1"))
    {
      mode = FitnessModel::FIXED_S; 
      // cout << "building fixedS" << endl; 
      fixedS = atof(args[1].c_str()); 
      
      float  tmpPos = atof(args[2].c_str()); 
      float  tmpNeg = atof(args[3].c_str()); 
      
      probNeutral = 1 - tmpPos - tmpNeg; 
      probPos = tmpPos  / (1 - probNeutral); 
    }
  else if( NOT modeString.compare("2") )
    {
      mode = FitnessModel::MIXTURE_GAMMA; 
      // cout << "building mixed gamma" << endl; 
      float  tmpPos = atof(args[1].c_str()); 
      float  tmpNeg = atof(args[4].c_str());  
      
      alpha[0] = atof(args[2].c_str()); 
      alpha[1] = atof(args[5].c_str()); 

      beta[0] = atof(args[3].c_str()); 
      beta[1] = atof(args[6].c_str()); 

      probNeutral =  1 - tmpNeg - tmpPos; 
      probPos = tmpPos / (1-probNeutral); 
    }
  else if( NOT modeString.compare("3") )
    {
      mode = FitnessModel::NORMAL; 
      // cout << "building normal distributtion" << endl; 
      FITNESS_TYPE tmpSel = atof(args[1].c_str());
      probNeutral = 1 - tmpSel; 
      // cout << "probability of neutral mutation is " << probNeutral << endl;       
      mean = atof(args[2].c_str());
      if(mean > 1 )
	{
	  cerr << "If selection coefficients are drawn from a normal distribution, the mean of this distribution cannot be > 1" << endl; 
	  abort(); 
	}

      sd = atof(args[3].c_str()); 
    }
  else if( NOT modeString.compare("4") )
    {
      mode = FitnessModel::FILE; 
      cerr << "not implemented yet" << endl; 
      abort();
    }
  else 
    {
      cerr << "not implemented yet" << endl; 
      abort();
    } 
}


seqLen_t FitnessFunction::getNumberSelected(seqLen_t totalLength)
{
  return seqLen_t((1-probNeutral)  * totalLength);
}
