AUTOMAKE_OPTIONS=foreign subdir-objects

WARN_FLAGS=-Wno-unused-result  -Wno-sign-compare -Wall  -Wno-unused-variable -Wextra -Wno-unused-parameter -fpermissive
LANG_FLAGS=-std=c++0x 


if WITH_DEBUG
 OPT=-O1 -g -funroll-all-loops -finline-functions -fomit-frame-pointer -DNDEBUG  
else 
 OPT=-O3 -funroll-all-loops -finline-functions -fomit-frame-pointer -DNDEBUG  
endif 

AM_CXXFLAGS = $(LANG_FLAGS) $(SIMD_FLAGS) $(WARN_FLAGS) $(OPT)  -Ilib/RandomLib/include -Ilib $(BOOST_CPPFLAGS)  -DHAVE_64BIT  # TODO get rid of 64 bit flag 


bin_PROGRAMS=Anafits convertSeq convertGraph
Anafits_LDADD=./lib/libasmlib.a $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIB) 


Anafits_SOURCES =  src/Ancestry.cpp src/Chromosome.cpp src/FreeRing.cpp src/HaploTimeWindow.cpp src/Mutation.cpp src/PopulationManager.cpp src/RecombinationManager.cpp src/SequenceArray.cpp src/Survivors.cpp src/TimeSection.cpp src/BinaryWriter.cpp src/common.cpp src/GenerationCounter.cpp src/HashTable.cpp src/MutationManager.cpp src/PopulationParams.cpp src/Region.cpp  src/System.cpp src/BitSet.cpp src/FitnessFunction.cpp  src/InfoFile.cpp src/Node.cpp src/ProgramOptions.cpp src/RegionManager.cpp src/SequenceFinalizer.cpp src/Thread.cpp src/ChromConfig.cpp src/FractionalSimulation.cpp src/Graph.cpp src/main.cpp src/NodeManager.cpp src/Randomness.cpp src/SeqRep.cpp src/Simulation.cpp src/ThreadPool.cpp lib/RandomLib/src/Random.cpp  

convertSeq_SOURCES = src/sequenceConversion.cpp src/BitSet.cpp
convertGraph_SOURCES = src/graphConversion.cpp

EXTRA_DIST=utils src/*.hpp lib/asmlib.h lib/libasmlib.a lib/RandomLib lib/cycle.hpp doc/manual.pdf lib/asmlib
