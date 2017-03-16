COMPILER = clang++
FLAGS = -O2 -std=c++11
DFLAGS = -g -std=c++11
TESTARGS = ../Data/Basic_Train.txt out_file.txt
ARGS = ../Data/Train.txt out_file.txt
FILENAMES = $(wildcard *.cpp)
OUTFILE=main.out

all: build
	./$(OUTFILE) $(ARGS) && cat out_file.txt
test: build
	./$(OUTFILE) $(TESTARGS) && cat out_file.txt	
build:
	$(COMPILER) $(FILENAMES) $(FLAGS) -o $(OUTFILE)
debug: debug_build
	gdb $(OUTFILE)
debug_build:
	$(COMPILER) $(FILENAMES) $(DFLAGS) -o $(OUTFILE)
clean:
	rm -f $(OUTFILE)

.PHONY: all test build debug clean
