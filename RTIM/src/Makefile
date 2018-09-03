# time -p OMP_NUM_THREADS=1 ./rtim -dataset test -model IC -stage pre
# nethept
CXX=g++ -O3 -std=c++14 #-fpermissive
EXEC=rtim

all: $(EXEC)

rtim: Arguments.o Graph.o RTIM.o Tools.o
	$(CXX) -fopenmp -o rtim Arguments.o Graph.o RTIM.o Tools.o

RTIM.o: RTIM.cpp Graph.cpp Arguments.cpp Graph.h RTIM.h Arguments.h Tools.cpp Tools.h
	$(CXX) -fopenmp -c RTIM.cpp

Graph.o: Graph.cpp Arguments.cpp Graph.h Arguments.h Tools.cpp Tools.h
	$(CXX) -fopenmp -c Graph.cpp

Arguments.o: Arguments.cpp Arguments.h Tools.cpp Tools.h
	$(CXX) -c Arguments.cpp

Tools.o: Tools.cpp Tools.h
	$(CXX) -c Tools.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
