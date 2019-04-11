CXX=g++ -O3 -std=c++0x
EXEC= main #imm_continuous imm_discrete

all: $(EXEC)

main: IMM.o InfGraph.o Graph.o Math.o Timer.o Counter.o OutputInfo.o sfmt.o head.o
	$(CXX) -o main IMM.o InfGraph.o Graph.o Math.o Timer.o Counter.o OutputInfo.o sfmt.o head.o

head.o: head.h head.c
	$(CXX) -c head.c -o head.o

sfmt.o: sfmt/SFMT.c
	$(CXX) -c sfmt/SFMT.c -o sfmt.o

IMM.o: IMM.h IMM.cpp
	$(CXX) -c IMM.cpp

Math.o: Math.h Math.cpp
	$(CXX) -c Math.cpp

Timer.o: Timer.h Timer.cpp
	$(CXX) -c Timer.cpp

OutputInfo.o: OutputInfo.h OutputInfo.cpp
	$(CXX) -c OutputInfo.cpp

Counter.o: Counter.h Counter.cpp
	$(CXX) -c Counter.cpp

InfGraph.o: InfGraph.h InfGraph.cpp
	$(CXX) -c InfGraph.cpp

Graph.o: Graph.h Graph.cpp
	$(CXX) -c Graph.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
