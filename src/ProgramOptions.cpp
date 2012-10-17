#include "ProgramOptions.hpp"
#include "common.hpp"

#include <vector>
#include <iostream>




void releaseText()
{
  cout << "This is a pre-release version of a fast forward-in-time population genetics simulator. " << endl; 
}


ProgramOptions::ProgramOptions(int argc, char **argv)  
  : desc("Options:")
{  
  po::options_description tuning("Tuning parameters");
  tuning.add_options()
    ("cleanF,Z", po::value<nat>()->default_value(100), "check every <x> generations for fixed mutations.")
    ("memory,M", po::value<string>(), "recommend  memory limit (e.g., specify 1G or 500M). Note: the same seed will not result in a reproducible run with varying values of -M. AnA-FiTS can not guarantee to stay below the limit.")
    ; 
  
  po::options_description prog("Program flags"); 
  prog.add_options()
    ("help,h", "produces this help message")
    ("version,v", "prints the version and exits")

    // :TODO:   
    // ("threads,p", po::value<nat>()->default_value(1) , "number of threads")
    ("runId,n", 
#ifdef PRODUCTIVE
     po::value<string>()->required(), 
#else 
     po::value<string>()->default_value("id"),
#endif
     "id for this run")
    ;


  po::options_description sim("Simulation parameters"); 
  sim.add_options()
    ("SIM,S", po::value<nat>()->default_value(5), "number of generations ( <value> x 2 x popSize)") 
    // ("burn,b", po::value<nat>()->default_value(2), "subsequent burn-in times ( x ploidy x popSize)")
    // ("ploidy,P", po::value<nat>()->default_value(2), "ploidy of all populations")
    ("seed,s", 
#ifdef PRODUCTIVE
     po::value<nat>()->required(), 
#else
     po::value<nat>()->default_value(123), 
#endif
     "random number seed")
    ; 


  po::options_description pop("Population-specific parameters"); 
  pop.add_options()
    // either specified directly or by id   
    ("popSize,N", po::value< vector<nat> >() ->default_value(vector<nat>(1,500U), "500") ->composing()->multitoken(), "initial ancestral population size") 
    ("mutRate,m",po::value< vector<string> >()->default_value(vector<string>(1,"0.00000001"), "1E-8")->multitoken(), "mutation rate (per site => mu)" ) // specfiy with P <id> for population specific rates
    ("recRate,r",po::value< vector<string> >()->default_value(vector<string>(1,"0.00000001"), "1E-8")->multitoken(), "recombination rate (per base)" )  // \nspecfiy with P <id> for population specific rates
    // ("gcRate,m", po::value< vector<string> >()->default_value(vector<string>(1,"0.0"), "0")->multitoken(), "gene conversion rate (per base)\nspecfiy with P <id> for population specific rates")    
    ("popEvent,T", po::value< vector<string> >()->multitoken()->composing() , 
     "timed event. \nFormat: '<c|d|g|k> <t> '\n\n"
     "Various events that affect the population size can be simulated. Effects take place in generation <t>. \nOptions:\n" // If [P <id>] is specified, only the respective population is affected, otherwise it applies to every population.\n
     "c t r   \tdiscrete size change. rate = new/old. Afterwards population size remains constant.\n"
     "d t r   \texponential decay with rate <r>\n"
     "g t r   \texponential growth with rate <r>.\n"
     "k t K r \tlogistic growth with capacity <K> and rate <r>\n"
     )
    ; 

  vector<string> defaultSelect({"2", 
	"0.05", "10", "2000",	// pos
	"0.05", "1", "500"}); 	// neg
  stringstream ss; 
  copy(defaultSelect.begin(), defaultSelect.end(), ostream_iterator<string>(ss, " ")); 
  po::options_description loc("Locus-specific parameters"); 
  loc.add_options()
    // no linkage at first 
    ("length,L", po::value< vector<seqLen_t> >()->multitoken()->default_value(vector<seqLen_t>(1,10000000), "10 Mbp"), "locus length (repeat for multiple unlinked chromosomes). Maximum: ~4.2Gbp" )
    ("neutral,w", po::value<bool>()->zero_tokens(), "simulate a neutral population (overrides contradictory flags, uses only the BEG algorithm)") // :TRICKY: for multiple pops, we'd need to use some tokens
    ("selCoef,W",
     po::value< vector<string> >()->default_value(defaultSelect, ss.str())->multitoken(), 
     "Distribution of selection coefficients: <mode> <args> \n"
     "1 <COEF> <p_pos> <p_neg>\ta single coefficient for positive/negative effects occurring with probability <pos> or <neg>\n"
     "2 <p_pos> <alpha> <beta> <n_neg> <alpha> <beta>\tmixture of gamma distributions\n" 
     "3 <p_sel> <mean> <sigma>\tnormal distribution\n"
     // "4 <file>\tnot implemented yet\n")
    // SFS: annotate locus as coding or sexual 
     ) ;

  desc.add(prog); 
  desc.add(sim); 
  desc.add(pop); 
  desc.add(loc); 
  desc.add(tuning); 

  po::store(po::parse_command_line(argc, argv, desc), vm);

  if (vm.count("help")) 
    {
      releaseText(); 
      cout << desc << "\n";
      exit(0);
    }
  
  if(vm.count("version"))
    {
      cout << "AnA-FiTS -- v." << VERSION  << endl; 
      exit(0); 
    }  

  // :BUG: did not work on hitssv109
  // try    
  //   {
  po::notify(vm); 
  //   }
  // catch (po::required_option requiredOption)
  //   {
  //     std::cerr << "Option > --" << requiredOption.get_option_name() << " < is mandatory! \n For help on how to specfiy it correctly, consider --help " << endl; 
  //     abort();
  //   }

  // :KLUDGE:
  if(this->get<vector<string>>("selCoef").size() > 7)
    {
      cerr << "Per chromosome selection cofficient distributions not implemented at the moment." << endl; 
      abort();
    }  
}


nat ProgramOptions::getNumberOfGenerations()
{
  vector<nat> initPopSizes = vm["popSize"].as<vector<nat> >(); 
  auto maximum = max_element(initPopSizes.begin(), initPopSizes.end());   
  nat burnin = vm["SIM"].as<nat>() ; 
  // nat ploidy  =vm["ploidy"].as<nat>();

  return 2 * burnin * *maximum; 
}
