# $Id$

REQUIREDHEADERS = Random.hpp RandomCanonical.hpp RandomPower2.hpp \
	RandomEngine.hpp RandomAlgorithm.hpp RandomMixer.hpp RandomSeed.hpp \
	RandomType.hpp

OTHERHEADERS = NormalDistribution.hpp ExponentialDistribution.hpp \
	LeadingZeros.hpp ExponentialProb.hpp RandomSelect.hpp \
	ExactExponential.hpp ExactNormal.hpp ExactPower.hpp RandomNumber.hpp

MPFRHEADERS = MPFRRandom.hpp \
	MPFRUniform.hpp MPFRExponential.hpp MPFRNormal.hpp \
	MPFRExponentialL.hpp MPFRNormalK.hpp MPFRNormalR.hpp

PREFIX = /usr/local
LIBNAME = RandomLib
HEADERS = $(LIBNAME)/Config.h \
	$(patsubst %,$(LIBNAME)/%,$(REQUIREDHEADERS) \
	$(OTHERHEADERS) $(MPFRHEADERS))
DEST = $(PREFIX)/include/$(LIBNAME)

INSTALL=install -b

all:
	@:

install:
	test -d $(DEST) || mkdir -p $(DEST)
	$(INSTALL) -m 644 $(HEADERS) $(DEST)/

list:
	@echo $(HEADERS)

clean:
	@:

.PHONY: all install list clean
