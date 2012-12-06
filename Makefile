VERSIONFILE=src/version.hpp
PROGNAME=AnA-FiTS

include system/Makefile.standard


# library flags 
LFLAGS_OPTPARS = -lboost_program_options
LFLAGS_GTEST = -Llib/gtest/lib/.libs -lgtest_main -lgtest
LFLAGS_GPROF = -lprofiler
LFLAGS_ASMLIB =  -lasmlib
LFLAGS =  -L./lib  $(LFLAGS_RAND) $(LFLAGS_OPTPARS) $(LFLAGS_LIKWID) $(LFLAGS_ASMLIB)  # -lpthread -ltcmalloc -ljemalloc #  $(LFLAGS_PANTH)   $(LFLAGS_GPROF) 

# include flags 
RAND_INCLUDE=-Ilib/RandomLib/include
INCLUDES = $(RAND_INCLUDE) -Ilib/gtest/include

LANG =  -Wno-sign-compare -Wall -std=c++0x  -Wno-unused-variable -Wextra -Wno-unused-parameter -fpermissive #  -Wstrict-aliasing=2  -Wfloat-equal -flto # is permissive a prolem?  

OPT = -O3 -march=native -mtune=native -funroll-all-loops #  -DNDEBUG

# specific 
DEBUG_CFLAGS = $(SSE_FLAG) $(FLAG_64) $(LANG) $(DEBUG) -ggdb 
DEBUG_LFLAGS = $(LFLAGS) 

CFLAGS = $(LANG) $(SSE_FLAG) $(FLAG_64) $(OPT) -fomit-frame-pointer

PROFILE_CFLAGS = $(SSE_FLAG) $(FLAG_64) $(LANG) $(OPT) -DPROFILE -ggdb -pg
PROFILE_LFLAGS = $(LFLAGS) $(LFLAGS_GPROF)

SRCDIR := src/
TESTSRC := tests/

SRC_EXCLUDE:=%sequenceConversion.cpp %graphConversion.cpp %_flymake.cpp %_flymake.hpp %_flymake_master.cpp

RAND_OBJ=lib/RandomLib/src/Random.o 
TEST_OBJS+=$(RAND_OBJ)

## a hack
firstTarget : release 

standardTargets :     convertSeq convertGraph   vupdate  cmpMessage depend

include system/Makefile.build


## building the rng 
$(RAND_OBJ):
	@echo [CXX] lib/RandomLib/src/Random.cpp
	@$(CXX)  -Wall -Wextra -O3 -funroll-loops -finline-functions -fomit-frame-pointer $(SSE_FLAG) $(RAND_INCLUDE) -c -o $@ lib/RandomLib/src/Random.cpp


clean:
	@echo "[CLEAN]"
	$(RM) $(OBJDIR) $(TEST_OBJ_DIR) $(DEBUG_OBJ_DIR) $(PROFILE_OBJ_DIR) $(ALL_TARGETS) $(RAND_OBJ) convertGraph convertSeq  *~ \#* callgrind* cachegrind*  gmon.out ffits.prof 

.PHONY : clean depend vupdate doc cmpMessage


vupdate : 
	@echo "[VERSION]"
	@echo "#define VERSION \"$(NEW_VERSION)\"" > src/version.hpp

# BEGIN  intermediate section for the convert tool
convertSeq : $(OBJDIR)/sequenceConversion.o $(OBJDIR)/BitSet.o     
	@echo [LINK] $@
	$(CXX) -o $@  $^ $(LFLAGS)  
convertGraph :  $(OBJDIR)/graphConversion.o
	@echo [LINK] $@
	$(CXX) -o $@ $^ $(LFLAGS)

## END 

doc: doc/manual.pdf


include system/Makefile.std-targets
include system/Makefile.depend
