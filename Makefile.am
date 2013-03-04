AUTOMAKE_OPTIONS=foreign

WARN_FLAGS=-Wno-unused-result  -Wno-sign-compare -Wall  -Wno-unused-variable -Wextra -Wno-unused-parameter -fpermissive
LANG_FLAGS= -std=c++0x 

OPT = -O3 -march=native -mtune=native -funroll-all-loops -finline-functions -fomit-frame-pointer  $(SIMD_FLAGS)  #  -DNDEBUG
AM_CXXFLAGS = $(LANG_FLAGS) $(WARN_FLAGS) $(OPT) -Ilib/RandomLib/include -Ilib $(BOOST_CPPFLAGS)  # -DHAVE_64BIT #


bin_PROGRAMS = Anafits convertSeq convertGraph
Anafits_LDADD=./lib/libasmlib.a $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIB) 


Anafits_SOURCES =  src/Ancestry.cpp src/Chromosome.cpp src/FreeRing.cpp src/HaploTimeWindow.cpp src/Mutation.cpp src/PopulationManager.cpp src/RecombinationManager.cpp src/SequenceArray.cpp src/Survivors.cpp src/TimeSection.cpp src/BinaryWriter.cpp src/common.cpp src/GenerationCounter.cpp src/HashTable.cpp src/MutationManager.cpp src/PopulationParams.cpp src/Region.cpp  src/System.cpp src/BitSet.cpp src/FitnessFunction.cpp  src/InfoFile.cpp src/Node.cpp src/ProgramOptions.cpp src/RegionManager.cpp src/SequenceFinalizer.cpp src/Thread.cpp src/ChromConfig.cpp src/FractionalSimulation.cpp src/Graph.cpp src/main.cpp src/NodeManager.cpp src/Randomness.cpp src/SeqRep.cpp src/Simulation.cpp src/ThreadPool.cpp lib/RandomLib/src/Random.cpp 

convertSeq_SOURCES = src/sequenceConversion.cpp src/BitSet.cpp
convertGraph_SOURCES = src/graphConversion.cpp



EXTRA_DIST=utils src/*.hpp




#  src/AddrArray.hpp     src/DynArray.hpp              src/InfoFile.hpp           src/Randomness.hpp            src/SequenceFinalizer.hpp
# src/Ancestry.hpp      src/DynArraySequential.hpp    src/JudyArray.hpp          src/RandomnessImpl.hpp        src/Simulation.hpp
# src/Base.hpp          src/Events.hpp                src/LoadBalancer.hpp       src/RandomnessImplSSE.hpp     src/SseWrapper.hpp
# src/BinaryWriter.hpp  src/FitnessFunction.hpp       src/Mutation.hpp           src/Recombination.hpp         src/StopWatch.hpp
# src/BitSet.hpp        src/FractionalSimulation.hpp  src/MutationManager.hpp    src/RecombinationManager.hpp  src/Survivors.hpp
# src/ChromConfig.hpp   src/FreeRing.hpp              src/Node.hpp               src/Region.hpp                src/System.hpp
# src/Chromosome.hpp    src/GenerationCounter.hpp     src/NodeManager.hpp        src/RegionManager.hpp         src/Thread.hpp
# src/common.hpp        src/Graph.hpp                 src/PopulationManager.hpp  src/ResizMemArena.hpp         src/ThreadPool.hpp
# src/config.hpp        src/HaploTimeWindow.hpp       src/PopulationParams.hpp   src/SeqRep.hpp                src/TimeSection.hpp
# src/CycleCounter.hpp  src/HashTable.hpp             src/ProgramOptions.hpp     src/SequenceArray.hpp         src/types.hpp
