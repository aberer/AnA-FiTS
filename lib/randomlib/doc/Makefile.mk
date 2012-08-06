# $Id$

REQUIREDHEADERS = Random.hpp RandomCanonical.hpp RandomPower2.hpp \
	RandomEngine.hpp RandomAlgorithm.hpp RandomMixer.hpp RandomSeed.hpp \
	RandomType.hpp

OTHERHEADERS = NormalDistribution.hpp ExponentialDistribution.hpp \
	LeadingZeros.hpp ExponentialProb.hpp RandomSelect.hpp \
	ExactExponential.hpp ExactNormal.hpp ExactPower.hpp RandomNumber.hpp

HEADERS = $(REQUIREDHEADERS) $(OTHERHEADERS)
SOURCES = Random.cpp

VPATH = ../include/RandomLib ../src

EXTRAFILES = exphist.pdf exphist.png powerhist.pdf powerhist.png

VERSION:=$(shell grep 'The current version of the library' ../NEWS | \
	sed -e 's/.* //' -e 's/\.$$//')

doc: html/index.html

html/index.html: doxyfile.in Random.doc \
	$(HEADERS) $(SOURCES) $(EXTRAFILES) $(HTMLMANPAGES)
	if test -d html; then rm -rf html/*; else mkdir html; fi
	cp -p $(EXTRAFILES) ../LICENSE.txt html/
	sed -e "s%@PROJECT_SOURCE_DIR@%..%g" \
	-e "s%@RandomLib_VERSION@%$(VERSION)%g" \
	doxyfile.in | doxygen -

PREFIX = /usr/local
DEST = $(PREFIX)/share/doc/RandomLib/html
INSTALL = install -b

install: html/index.html
	test -d $(DEST) || mkdir -p $(DEST)
	$(INSTALL) -m 644 html/* $(DEST)/
list:
	@echo doxyfile Random.doc $(EXTRAFILES)

maintainer-clean:
	rm -rf html

.PHONY: doc install list clean
