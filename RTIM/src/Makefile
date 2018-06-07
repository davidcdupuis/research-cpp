CXX=g++
EXEC=rtim

all: $(EXEC)

rtim: Arguments.o Graph.o RTIM.o
	$(CXX) -o rtim src/Arguments.o src/Graph.o RTIM.o

RTIM.o: src/RTIM.cpp
	$(CXX) -c src/RTIM.cpp

Graph.o: src/Graph.cpp
	$(CXX) -c src/Graph.cpp

Arguments.o: src/Arguments.cpp
	$(CXX) -c src/Arguments.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
