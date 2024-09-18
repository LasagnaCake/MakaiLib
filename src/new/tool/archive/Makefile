import ../../../../commons.make

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	$(COMPILER) $(DEBUGCOMPCFG) -c archive.cpp -o $(prefix).archive.$@.o


release:
	$(COMPILER) $(COMPCFG) -c archive.cpp -o $(prefix).archive.$@.o
