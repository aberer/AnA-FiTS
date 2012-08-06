# $Id$

PROGRAMS = RandomExample RandomSave RandomThread \
	RandomCoverage RandomExact RandomLambda RandomTime

VSPROJECTS = $(addsuffix -vc8.vcproj,Random $(PROGRAMS)) \
	$(addsuffix -vc9.vcproj,Random $(PROGRAMS))

all:
	@:
install:
	@:
clean:
	@:
list:
	@echo $(VSPROJECTS) \
	RandomLib-vc8.sln RandomLib-vc9.sln

.PHONY: all install list clean
