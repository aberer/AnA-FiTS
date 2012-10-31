# $Id$

DEST = $(PREFIX)/share/cmake/RandomLib

INSTALL=install -b

all:
	@:
install:
	test -d $(DEST) || mkdir -p $(DEST)
	$(INSTALL) -m 644 FindRandomLib.cmake $(DEST)
list:
	@echo FindRandomLib.cmake \
	randomlib-config.cmake.in randomlib-config-version.cmake.in
clean:
	@:

.PHONY: all install list clean
