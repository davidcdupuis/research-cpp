CXX=g++ -O3 -std=c++0x
EXEC= main #imm_continuous imm_discrete

all: $(EXEC)

main: IMM.o InfGraph.o Graph.o Timer.o Counter.o sfmt.o head.o Arguments.o
	$(CXX) -o main IMM.o InfGraph.o Graph.o Timer.o Counter.o sfmt.o head.o Arguments.o

head.o: head.h head.c
	$(CXX) -c head.c -o head.o

sfmt.o: sfmt/SFMT.c
	$(CXX) -c sfmt/SFMT.c -o sfmt.o

IMM.o: IMM.h IMM.cpp
	$(CXX) -c IMM.cpp

Timer.o: Timer.h Timer.cpp
	$(CXX) -c Timer.cpp

Counter.o: Counter.h Counter.cpp
	$(CXX) -c Counter.cpp

InfGraph.o: InfGraph.h InfGraph.cpp
	$(CXX) -c InfGraph.cpp

Graph.o: Graph.h Graph.cpp
	$(CXX) -c Graph.cpp

Arguments.o: Arguments.h Arguments.cpp
	$(CXX) -c Arguments.cpp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
