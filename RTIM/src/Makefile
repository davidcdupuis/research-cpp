CXX=g++
EXEC=rtim

all: $(EXEC)

rtim: Arguments.o Graph.o RTIM.o
	$(CXX) -o rtim Arguments.o Graph.o RTIM.o

RTIM.o: RTIM.cpp Graph.cpp Graph.h RTIM.h
	$(CXX) -c RTIM.cpp

Graph.o: Graph.cpp Graph.h
	$(CXX) -c Graph.cpp

Arguments.o: Arguments.cpp Arguments.h
	$(CXX) -c Arguments.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
