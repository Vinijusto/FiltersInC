.PHONY: all clean

BIN = tp

CPP = main.cpp filters.cpp ppm.cpp
OBJ = $(CPP:.cpp=.o)
SRC = $(CPP)

all: tp

clean:
	rm -f $(BIN) $(OBJ)

tp: main.o filters.o ppm.o
	g++ -pthread -o $@ $^

%.o: %.cpp
	g++ -pthread -c -o $@ $<
