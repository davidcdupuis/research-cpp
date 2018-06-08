CXX=g++
EXEC=rtim

all: $(EXEC)

rtim: Arguments.o Graph.o RTIM.o
	$(CXX) -o rtim Arguments.o Graph.o RTIM.o

RTIM.o: RTIM.cpp
	$(CXX) -c RTIM.cpp

Graph.o: Graph.cpp
	$(CXX) -c Graph.cpp

Arguments.o: Arguments.cpp
	$(CXX) -c Arguments.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
