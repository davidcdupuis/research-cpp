CXX=g++
EXEC=rtim

all: $(EXEC)

rtim: src/*.cpp src/*.h
	$(CXX) src/RTIM.cpp -o rtim

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
