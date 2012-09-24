Q ?= @
CCACHE := $(shell which ccache)
CXX :=  $(Q) $(CCACHE) $(lastword $(wildcard /usr/bin/g++-* )) 
RM := $(Q)rm -fr 
GITVERSION := $(shell git log | grep Date  | cut -f 2,3 -d ':' | sed 's/[ ]*....\(.*\)/\1/' | tr -d ':' | tr ' ' '-' | head -n 1 )

PROGNAME=AnA-FiTS

# compiles with sse2 support, if available 
HAVE_SSE2 = $(shell grep "flags\b.*\bsse2\b" /proc/cpuinfo 2> /dev/null | tail -1 | wc -l | tr -d ' \t')
ifeq ($(HAVE_SSE2),1)
SSE_FLAG = -msse2 -DHAVE_SSE2
endif

# conditionally compile with 64-bit support  
HAVE_64BIT =  $(shell grep "flags\b.*\blm\b" /proc/cpuinfo 2> /dev/null | tail -1 | wc -l | tr -d ' \t')
ifeq ($(HAVE_64BIT),1)
FLAG_64 += -DHAVE_64BIT
endif

LFLAGS_OPTPARS = -lboost_program_options
LFLAGS_GTEST = -Llib/gtest/lib/.libs -lgtest_main -lgtest
LFLAGS_GPROF = -lprofiler
LFLAGS_ASMLIB =  -lasmlib
LFLAGS =  -L./lib  $(LFLAGS_RAND) $(LFLAGS_OPTPARS) $(LFLAGS_LIKWID) $(LFLAGS_ASMLIB)  # -lpthread -ltcmalloc -ljemalloc #  $(LFLAGS_PANTH)   $(LFLAGS_GPROF) 

RAND_INCLUDE=-Ilib/RandomLib/include
INCLUDES = $(RAND_INCLUDE) -Ilib/gtest/include

#  -flto
# -DNDEBUG 
LANG =  -Wno-sign-compare -Wall -std=c++0x  -Wno-unused-variable -Wextra -Wno-unused-parameter -fpermissive #  -Wstrict-aliasing=2  -Wfloat-equal # is permissive a prolem?  
OPT = -O3 -march=native -mtune=native -funroll-all-loops 
OPT_UNKNOWN = -D_HAS_EXCEPTIONS=0  -D_STATIC_CPPLIB -ffast-math -fno-common

# specific 
DEBUG_CFLAGS = $(SSE_FLAG) $(FLAG_64) $(LANG) $(DEBUG) -ggdb 
DEBUG_LFLAGS = $(LFLAGS) 

CFLAGS =  $(SSE_FLAG) $(FLAG_64) $(LANG) $(OPT) $(FLAG_64) -fomit-frame-pointer

PROFILE_CFLAGS = $(SSE_FLAG) $(FLAG_64) $(LANG) $(OPT) -DPROFILE -ggdb -pg
PROFILE_LFLAGS = $(LFLAGS) $(LFLAGS_GPROF)

RAND_OBJ=lib/RandomLib/src/Random.o 

# ================================================================ 
# DO NOT CHANGE CONTENTS BELOW THIS LINE 

# targets 
TARGET_TEST := test-$(PROGNAME)-$(GITVERSION)
TARGET_DEBUG := debug-$(PROGNAME)-$(GITVERSION)
TARGET_PROFILE := profile-$(PROGNAME)-$(GITVERSION)
TARGET := $(PROGNAME)-$(GITVERSION)
ALL_TARGETS := $(TARGET) $(TARGET_DEBUG) $(TARGET_PROFILE) $(TARGET_TEST)

SRCDIR := src/
TESTSRC := tests/
OBJDIR := build/release-$(GITVERSION)/
TEST_OBJ_DIR := build/test-$(GITVERSION)/
DEBUG_OBJ_DIR := build/debug-$(GITVERSION)/
PROFILE_OBJ_DIR := build/profile-$(GITVERSION)/

SRC_FILES := $(filter-out %_flymake.cpp %_flymake.hpp %_flymake_master.cpp, $(wildcard $(SRCDIR)/*.cpp))
TEST_FILES := $(filter-out %_flymake.cpp %_flymake.hpp %_flymake_master.cpp, $(wildcard $(TESTSRC)/*.cpp))

OBJS := $(addprefix $(OBJDIR),  $(notdir $(SRC_FILES))) 
OBJS := $(OBJS:.cpp=.o) $(RAND_OBJ)

TEST_OBJS := $(addprefix $(TEST_OBJ_DIR), $(filter-out main.cpp, $(notdir $(SRC_FILES)))) $(addprefix $(TEST_OBJ_DIR),  $(notdir $(TEST_FILES)))
TEST_OBJS := $(TEST_OBJS:.cpp=.o)

DEBUG_OBJS := $(addprefix $(DEBUG_OBJ_DIR),  $(notdir $(SRC_FILES)))
DEBUG_OBJS := $(DEBUG_OBJS:.cpp=.o)

PROFILE_OBJS := $(addprefix $(PROFILE_OBJ_DIR),  $(notdir $(SRC_FILES)))
PROFILE_OBJS := $(PROFILE_OBJS:.cpp=.o)

release :  $(OBJDIR) $(TARGET) 
dist:  depend  $(OBJDIR) $(TARGET) 
all : depend release   test #  debug  # profile

debug: depend  $(DEBUG_OBJ_DIR) $(TARGET_DEBUG)
profile: depend $(PROFILE_OBJ_DIR) $(TARGET_PROFILE)
test : depend $(TEST_OBJ_DIR) $(TARGET_TEST)
	./$(TARGET_TEST)

cppcheck: 
	cppcheck --force --inline-suppr  --quiet --enable=all --template=gcc src 

runProfile:  profile 
	@echo [GOOGLE-PROF]
	@./profile-ffits-$(GITVERSION) > /dev/zero 2>/dev/zero
	@echo [DISPLAYING]
	@LANG=C google-pprof --nodefraction=0.02 --web ./profile-ffits-$(GITVERSION) ./ffits.prof > /dev/zero 2> /dev/zero
	@echo [VALGRIND]
	@valgrind -q  --tool=callgrind ./profile-ffits-$(GITVERSION)   > /dev/zero 

check-syntax:
	$(CXX) $(LANG) -Wstrict-overflow -Wall -Wextra -pedantic -fsyntax-only $(INCLUDES) $(CHK_SOURCES) 

$(RAND_OBJ) : 
	@echo [CXX] lib/randomlib/src/Random.cpp
	@$(CXX) -g -Wall -Wextra -O3 -funroll-loops -finline-functions -fomit-frame-pointer $(SSE_FLAG) $(RAND_INCLUDE) -c -o $@ lib/randomlib/src/Random.cpp

$(OBJDIR):
	@mkdir -p $@
$(TEST_OBJ_DIR) :
	@mkdir -p $@
$(DEBUG_OBJ_DIR):
	@mkdir -p $@
$(PROFILE_OBJ_DIR):
	@mkdir -p $@

# compile files 
$(OBJDIR)%.o : $(SRCDIR)%.cpp 
	@echo [CXX] $@
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(TEST_OBJ_DIR)%.o : $(SRCDIR)%.cpp 
	@echo [CXX] $@
	$(CXX) -DTESTING_ENVIRONMENT $(DEBUG_CFLAGS) $(INCLUDES) -c -o $@ $<

$(TEST_OBJ_DIR)%.o : $(TESTSRC)%.cpp  
	@echo [CXX] $@
	$(CXX) $(DEBUG_CFLAGS) $(INCLUDES) -c -o $@ $<

$(DEBUG_OBJ_DIR)%.o : $(SRCDIR)%.cpp 
	@echo [CXX] $@
	$(CXX) $(DEBUG_CFLAGS) $(INCLUDES) -c -o $@ $<  $(LFLAGS)

$(PROFILE_OBJ_DIR)%.o : $(SRCDIR)%.cpp 
	@echo [CXX] $@
	$(CXX) $(PROFILE_CFLAGS) $(INCLUDES) -c -o $@ $<


# linking binaries 
$(TARGET_TEST): $(TEST_OBJS) $(RAND_OBJ)
	@echo [LINK] $@
	$(CXX)  -DTESTING_ENVIRONMENT -o $@  $^ $(LFLAGS) $(LFLAGS_GTEST) 

$(TARGET): $(OBJS)  $(RAND_OBJ)
	@echo [LINK] $@
	$(CXX) -o $@  $^ $(LFLAGS)  

$(TARGET_DEBUG):  $(DEBUG_OBJS) $(RAND_OBJ)
	@echo [LINK] $@
	$(CXX) -o $@ $^ $(DEBUG_LFLAGS)

$(TARGET_PROFILE) : $(PROFILE_OBJS)   $(RAND_OBJ)
	@echo [LINK] $@
	$(CXX) -o $@ $^  $(PROFILE_LFLAGS)

clean:
	@echo "[CLEAN]"
	$(RM) $(OBJDIR) $(TEST_OBJ_DIR) $(DEBUG_OBJ_DIR) $(PROFILE_OBJ_DIR) $(ALL_TARGETS) $(RAND_OBJ) *~ \#* callgrind* cachegrind*  gmon.out ffits.prof

.PHONY : clean depend

DEPFILE  =  .depends
DEPTOKEN = '\# MAKEDEPENDS'
DEPFLAGS = -Y -f $(DEPFILE) -s $(DEPTOKEN) 

depend :
	@echo [DEPEND]
	@rm -f $(DEPFILE)
	@echo $(DEPTOKEN) > $(DEPFILE)
	@makedepend $(DEPFLAGS) -a -p build/release-$(GITVERSION)/ -- $(CFLAGS) -- $(SRC_FILES)  > /dev/zero 2> /dev/zero
	@makedepend $(DEPFLAGS) -a  -p build/debug-$(GITVERSION)/ -- $(CFLAGS) -- $(SRC_FILES)  > /dev/zero 2> /dev/zero
	@makedepend $(DEPFLAGS) -a  -p build/test-$(GITVERSION)/ -- $(CFLAGS) -- $(SRC_FILES) $(TEST_FILES)  > /dev/zero 2> /dev/zero
	@makedepend $(DEPFLAGS) -a  -p build/profile-$(GITVERSION)/ -- $(CFLAGS) -- $(SRC_FILES)  > /dev/zero 2> /dev/zero
	@cat $(DEPFILE) | sed "s/\/src\///" | sed "s/\/tests\///" > tmp 
	@mv tmp $(DEPFILE)
	@cd src;  ctags -Re * 

sinclude $(DEPFILE) 
