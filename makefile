IDIR =.
GTESTIDIR =/home/geri/workspace/googletest/googletest/include
GTESTLIB=/home/geri/workspace/googletest/build/googletest
CC=g++
CFLAGS=-I$(GTESTIDIR) -L$(GTESTLIB) -O2 -DDEBUG -pthread -g --std=c++11

ODIR=../obj

LIBS=-lm


cout: main.cpp test.cpp
	g++ $(CFLAGS) -o ../bin/dicdash main.cpp 
	g++ $(CFLAGS) -o ../tests/test-driver test.cpp -lpthread -lgtest -lgtest_main

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
