CC = g++
CFLAGS=-g -Wall -pedantic
#vitesse
CFLAGS+= -O0
# sfml options
#LFLAGS= -lsfml-graphics -lsfml-window -lsfml-system

all: test

test.o: test.cc
	$(CC) $(CFLAGS) -c $^

test: test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

clean:
	rm *.o
