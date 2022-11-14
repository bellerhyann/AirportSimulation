DEBUG = -g
OPTS = $(DEBUG) -Wall -ansi -pedantic -std=c++20

CC=g++ $(OPTS) -c
LN=g++

OBJS = planeTest.o AirportAnimator.o 

proj2: $(OBJS)
	$(LN) -o proj2 $(OBJS) -lpthread -lncurses

planeTest.o: planeTest.cpp AirportAnimator.hpp
	$(CC) planeTest.cpp

AirportAnimator.o: AirportAnimator.cpp
	$(CC) AirportAnimator.cpp



clean:
	/bin/rm -rf *~ $(OBJS) proj2
