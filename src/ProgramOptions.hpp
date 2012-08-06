#pragma once 

#include "common.hpp" 
#include "types.hpp"
#include "config.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

// TODO config file

class ProgramOptions
{
public: 
  ProgramOptions(int argc, char **argv);
  nat getNumberOfGenerations();  
  template<typename T> T get(string name) const {return vm[name].as<T>(); }
  bool hasOption(string name) const {return vm.count(name)  > 0 ; }

private: 
  po::options_description desc; 
  po::variables_map vm;
}; 
