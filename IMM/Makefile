CXX=g++ -O3 -Wall -std=c++0x
EXEC= main #imm_continuous imm_discrete

all: $(EXEC)

#imm_continuous: src/*.cpp src/*.h
#	$(CXX) -DCONTINUOUS src/IMM.cpp src/sfmt/SFMT.c  -o imm_continuous

#imm_discrete: src/*.cpp src/*.h
#	$(CXX) -DDISCRETE src/IMM.cpp src/sfmt/SFMT.c  -o imm_discrete

#imm: src/*.cpp src/*.h
#	$(CXX) src/imm.cpp src/sfmt/SFMT.c  -o imm

O0:
	g++ src/imm.cpp -Wall -std=c++0x -O0 src/sfmt/SFMT.c  -o imm
O1:
	g++ src/imm.cpp -Wall -std=c++0x -O1 src/sfmt/SFMT.c  -o imm
O2:
	g++ src/imm.cpp -Wall -std=c++0x -O2 src/sfmt/SFMT.c  -o imm
profile:
	g++ src/imm.cpp -Wall -std=c++0x -O2 src/sfmt/SFMT.c  -pg -o imm.profile

cont_profile:
	g++ -DCONTINUOUS src/IMM.cpp -Wall -std=c++0x -O3 src/sfmt/SFMT.c  -o imm_continuous -pg -o imm_continuous.profile

main: src/IMM.o src/InfGraph.o src/Graph.o src/Math.o src/Timer.o src/Counter.o src/OutputInfo.o
	$(CXX) -o main src/IMM.o src/InfGraph.o src/Graph.o src/Math.o src/Timer.o src/Counter.o src/OutputInfo.o

IMM.o: src/IMM.h src/IMM.cpp
	$(CXX) -c src/IMM.cpp

Math.o: src/Math.h src/Math.cpp
	$(CXX) -c src/Math.cpp

Timer.o: src/Timer.h src/Timer.cpp
	$(CXX) -c src/Timer.cpp

OutputInfo.o: src/OutputInfo.h src/OutputInfo.cpp
	$(CXX) -c src/OutputInfo.cpp

Counter.o: src/Counter.h src/Counter.cpp
	$(CXX) -c src/Counter.cpp

InfGraph.o: src/InfGraph.h src/InfGraph.cpp
	$(CXX) -c src/InfGraph.cpp

Graph.o: src/Graph.h src/Graph.cpp
	$(CXX) -c src/Graph.cpp

clean:
	rm -rf src/*.o

mrproper: clean
	rm -rf $(EXEC)
