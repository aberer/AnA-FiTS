#include <mutex>
#include <cassert>

class LoadBalancer
{
public: 
  LoadBalancer(nat _totalJobs)
  : totalJobs(_totalJobs)
  , currentJob(0)
  {
  }


  bool getTicket(nat &result)
  { 
    bool workToDo = false; 
    jobMutex.lock();
    result = currentJob; 
    workToDo = currentJob < totalJobs; 
    currentJob++; 
    jobMutex.unlock();
    return workToDo; 
  }

  void reset() { currentJob = 0; }  
  nat getTotalJobs() { return totalJobs; }
  void setNumberOfJobs(nat jobs){totalJobs = jobs;  assert(totalJobs == jobs); }
  bool isLast(nat jobId){ return totalJobs - 1 == jobId; } 

private:
  nat totalJobs; 
  nat currentJob; 
  mutex jobMutex; 
}; 
