# Project: New Text Stream

#CPP      = clang++ -stdlib=libstdc++ -lstdc++ -std=c++11
CPP      = g++ 
CC       = gcc
OBJ      = TestTextStream.o $(RES)
LINKOBJ  = TestTextStream.o $(RES)
BIN      = TestTextStream
CXXFLAGS = -Wall -g -O1 
CFLAGS   = -Wall -g -O1  
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before TestTextStream all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "TestTextStream" $(LIBS)

TestTextStream.o: TestTextStream.cpp
	$(CPP) -c TestTextStream.cpp -o TestTextStream.o $(CXXFLAGS)
