# time -p OMP_NUM_THREADS=1 ./rtim -dataset test -model IC -stage pre
# nethept
CXX=g++ -O3 -std=c++14 #-fpermissive
EXEC= main rtim

all: $(EXEC)

rtim: Graph.o RTIM.o Tools.o InfScore.o
	$(CXX) -fopenmp -o rtim Graph.o RTIM.o Tools.o InfScore.o

main: Graph.o RTIM_WMAIN.o Tools.o Main.o InfScore.o
	$(CXX) -fopenmp -o main Graph.o RTIM_WMAIN.o Tools.o Main.o InfScore.o

RTIM_WMAIN.o: RTIM.cpp RTIM.h # Graph.cpp Graph.h RTIM.h Tools.cpp Tools.h
	$(CXX) -DMAIN -fopenmp -c RTIM.cpp -o RTIM_WMAIN.o

InfScore.o: InfScore.cpp InfScore.h
	$(CXX) -fopenmp -c InfScore.cpp

RTIM.o: RTIM.cpp RTIM.h # Graph.cpp Graph.h RTIM.h Tools.cpp Tools.h
	$(CXX) -fopenmp -c RTIM.cpp -o RTIM.o

Graph.o: Graph.cpp Graph.h # Tools.cpp Tools.h
	$(CXX) -fopenmp -c Graph.cpp

Tools.o: Tools.cpp Tools.h
	$(CXX) -c Tools.cpp

Main.o: Main.cpp Main.h # Tools.cpp Tools.h Graph.cpp Graph.h RTIM.cpp RTIM.h
	$(CXX) -fopenmp -c Main.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
