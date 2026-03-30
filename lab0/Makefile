CXX := g++
CXXFLAGS := -O2 -fopenmp
SIZE ?= 16384

.PHONY: all clean run1 run4

all: proj00_1 proj00_4

proj00_1: proj00.cpp
	$(CXX) $(CXXFLAGS) -DNUMT=1 -DSIZE=$(SIZE) proj00.cpp -o $@

proj00_4: proj00.cpp
	$(CXX) $(CXXFLAGS) -DNUMT=4 -DSIZE=$(SIZE) proj00.cpp -o $@

run1: proj00_1
	./proj00_1

run4: proj00_4
	./proj00_4

clean:
	rm -f proj00_1 proj00_4
