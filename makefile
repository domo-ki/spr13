CXXFLAGS = $(shell pkg-config --cflags opencv)
LDFLAGS = $(shell pkg-config --libs opencv)
PROGRAM = color_test
LD = g++
CXX = g++
GXX = g++
CC = g++

default: $(PROGRAM)

.cpp.o:
  $(CXX) $(CXXFLAGS) -c -o $*.o $<

$(PROGRAM): $(PROGRAM).o
	$(CXX) $(PROGRAM).o -o $(PROGRAM) $(LDFLAGS) 

clean:
	/bin/rm *.o $(PROGRAM)
