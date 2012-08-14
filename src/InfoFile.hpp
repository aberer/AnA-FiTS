#pragma once 

#include "common.hpp"

#include <sstream>
#include <string>
#include <cstdio>
#include <cstdarg>


class InfoFile 
{
public:
  InfoFile(string id); 
  ~InfoFile();
  void write(const char *fmt,  ...);   

private:
  FILE *fh; 
}; 
