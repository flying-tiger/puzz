CXXFLAGS += --std=c++20 -O2 -DNDEBUG

puzz: puzz.cpp

.PHONY: clean
clean:
	rm puzz

