#include "InfoFile.hpp"


InfoFile::InfoFile(string id)
{
  stringstream fileName; 
  fileName << INF_FILE_NAME << "." << id; 
  fh =  openFile(fileName.str(), "w"); 
}


InfoFile::~InfoFile()
{
  fclose(fh); 
} 


void InfoFile::write(const char *fmt, ...)
{
  va_list args;
  va_start(args,fmt); 
  vfprintf(fh, fmt, args); 
#ifdef CONSOLE_OUTPUT
  va_start(args,fmt); 
  vprintf(fmt,args); 
#endif
  va_end(args); 
}

