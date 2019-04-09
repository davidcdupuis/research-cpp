CXX=g++ -03 -Wall -std=c++0x
EXEC= imm_continuous imm_discrete main

all: $(EXEC)

imm_continuous: src/*.cpp src/*.h
	$(CXX) -DCONTINUOUS src/imm.cpp src/sfmt/SFMT.c  -o imm_continuous

imm_discrete: src/*.cpp src/*.h
	$(CXX) -DDISCRETE src/imm.cpp src/sfmt/SFMT.c  -o imm_discrete

imm: src/*.cpp src/*.h
	$(CXX) src/imm.cpp src/sfmt/SFMT.c  -o imm
O0:
	g++ src/imm.cpp -Wall -std=c++0x -O0 src/sfmt/SFMT.c  -o imm
O1:
	g++ src/imm.cpp -Wall -std=c++0x -O1 src/sfmt/SFMT.c  -o imm
O2:
	g++ src/imm.cpp -Wall -std=c++0x -O2 src/sfmt/SFMT.c  -o imm
profile:
	g++ src/imm.cpp -Wall -std=c++0x -O2 src/sfmt/SFMT.c  -pg -o imm.profile

cont_profile:
	g++ -DCONTINUOUS src/imm.cpp -Wall -std=c++0x -O3 src/sfmt/SFMT.c  -o imm_continuous -pg -o imm_continuous.profile

main: IMM.o InfGraph.o Graph.o Arguments.o Math.o Timer.o Counter.o OutputInfo.o
	$(CXX) main -o IMM.o InfGraph.o Graph.o Arguments.o Math.o Timer.o Counter.o OutputInfo.o

IMM.o: IMM.h IMM.cpp
	$(CXX) IMM.cpp -o IMM.o

Arguments.o: Arguments.h Arguments.cpp
	$(CXX) Arguments.cpp Arguments.o

Math.o: Math.h Math.cpp
	$(CXX) Math.cpp -o Math.o

Timer.o: Timer.h Timer.cpp
	$(CXX) Timer.cpp -o Timer.o

OutputInfo.o: OutputInfo.h OutputInfo.cpp
	$(CXX) OutputInfo.cpp -o OutputInfo.o

Counter.o: Counter.h Counter.cpp
	$(CXX) Counter.cpp -o Counter.o

InfGraph.o: InfGraph.h InfGraph.cpp
	$(CXX) InfGraph.cpp -o InfGraph.o

Graph.o: Graph.h Graph.cpp
	$(CXX) Graph.cpp -o Graph.o

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
