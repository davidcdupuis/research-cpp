# time -p OMP_NUM_THREADS=1 ./rtim -dataset test -model IC -stage pre
# nethept
CXX=g++ -O3 -std=c++14 #-fpermissive
EXEC=rtim main

all: $(EXEC)

# rtim: Arguments.o Graph.o RTIM.o Tools.o
# 	$(CXX) -fopenmp -o rtim Arguments.o Graph.o RTIM.o Tools.o

main: Graph.o RTIM.o Tools.o Main.o
	$(CXX) -fopenmp -o main Graph.o RTIM.o Tools.o Main.o

RTIM.o: RTIM.cpp Graph.cpp Graph.h RTIM.h Tools.cpp Tools.h
	$(CXX) -fopenmp -c RTIM.cpp

Graph.o: Graph.cpp Graph.h Tools.cpp Tools.h
	$(CXX) -fopenmp -c Graph.cpp

Tools.o: Tools.cpp Tools.h
	$(CXX) -c Tools.cpp

Main.o: Main.cpp Main.h RTIM.cpp RTIM.h Tools.cpp Tools.h Graph.cpp Graph.h
	$(CXX) -fopenmp -c Main.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
