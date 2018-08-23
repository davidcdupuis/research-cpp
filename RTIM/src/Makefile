# time -p OMP_NUM_THREADS=1 ./rtim -dataset test -model IC -stage pre
# nethept
CXX=g++ -O3 -std=c++14 #-fpermissive
EXEC=rtim

all: $(EXEC)

rtim: Arguments.o Graph.o RTIM.o
	$(CXX) -fopenmp -o rtim Arguments.o Graph.o RTIM.o

RTIM.o: RTIM.cpp Graph.cpp Graph.h RTIM.h Arguments.h
	$(CXX) -fopenmp -c RTIM.cpp

Graph.o: Graph.cpp Graph.h Arguments.h
	$(CXX) -fopenmp -c Graph.cpp

Arguments.o: Arguments.cpp Arguments.h
	$(CXX) -c Arguments.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
