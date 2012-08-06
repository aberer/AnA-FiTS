#include "System.hpp"
#include <iostream>
#include <sys/resource.h>
// #include <proc/readproc.h>


#include "../lib/cycle.hpp"

using namespace std; 



System::System()
{
  who = RUSAGE_SELF;
  // sigar_open(&sigar);
  // ownPid =sigar_pid_get( sigar); 
}

System::~System()
{
  // sigar_close(sigar); 
}


void System::getFreeMem()
{
  // sigar_mem_t *mem = reinterpret_cast<sigar_mem_t*>(malloc(sizeof(sigar_mem_t))); 
  // sigar_mem_get(sigar, mem);
  // cout << "mem-ram: " << mem->ram 
  //      << "\nmem-total: " << mem->total / (1000 * 1000) << " MB"
  //      << "\nmem-used: " << mem->used / (1000 * 1000) <<  " MB"
  //      << "\nmem-free: " << mem->free / (1000 * 1000) <<  " MB"

  //   // that's the important one 
  //      << "\nmem-act-used: " << mem->actual_used / (1000 * 1000) <<  " MB"

  //      << "\nmem-act-free: " << mem->actual_free / (1000 * 1000) <<  " MB"
  //      << "\nmem-use_perc: " << mem->used_percent
  //      << "\nmem-fre_perc: " << mem->free_percent
  //      << endl; 
}

void System::getProcMem()
{
 //  sigar_proc_mem_t *procmem =  reinterpret_cast<sigar_proc_mem_t*>(malloc(sizeof(sigar_proc_mem_t))); 
 //  sigar_proc_mem_get(sigar, ownPid, procmem);

 //  // size is total 
 // cout <<  "size: "  << procmem->size / 1024 << " kB"

 //    // is in RAM 
 //       << "\nresident: " << procmem->resident  / 1024 << " kB"
    
 //    // is in virtual
 //       << "\nshare: " <<  procmem->share  / 1024 << " kB"
 //       << "\nminor_faults: " << procmem->minor_faults
 //       << "\nmajor_faults: " << procmem->major_faults
 //       << "\npage_faults: " << procmem->page_faults
 //       << endl; 
}


void System::process(char *when)
{
  getrusage(who, &usage);
  cout <<  when<< endl;
  cout << "user time used\t" <<   usage.ru_utime.tv_sec << "\t" << usage.ru_utime.tv_usec   << endl;
  cout << "system time used\t" <<   usage.ru_stime.tv_sec<<  "\t" << usage.ru_stime.tv_usec   << endl;
  cout << "integral shared memory size\t" <<   usage.ru_ixrss           << endl;
  cout << "integral unshared data\t" <<   usage.ru_idrss           << endl;
  cout << "integral unshared stack\t" <<   usage.ru_isrss           << endl;
  cout << "page reclaims\t" <<   usage.ru_minflt          << endl;
  cout << "page faults\t" <<   usage.ru_majflt          << endl;
  cout << "swaps\t" <<   usage.ru_nswap           << endl;
  cout << "block input operations\t" <<   usage.ru_inblock         << endl;
  cout << "block output operations\t" <<   usage.ru_oublock         << endl;
  cout << "messages sent\t" <<   usage.ru_msgsnd          << endl;
  cout << "messages received\t" <<   usage.ru_msgrcv          << endl;
  cout << "signals received\t" <<   usage.ru_nsignals        << endl;
  cout << "voluntary context switches\t" <<   usage.ru_nvcsw           << endl;
  cout << "involuntary\t" <<   usage.ru_nivcsw          << endl;

}



// also something like the peak mem usage ; not recommendable
long System::getTotalSystemMemory()
{
    // long pages = sysconf(_SC_PHYS_PAGES);
    // long page_size = sysconf(_SC_PAGE_SIZE);  
    // return pages * page_size;
  return 0; 
}




double System::getMaxMemConsumption()
{
  getrusage(who, &usage);
  return usage.ru_maxrss; 
}

