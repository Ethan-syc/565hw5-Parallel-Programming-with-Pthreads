CC = g++
CFLAGS = -O3 -std=c++11

all: rainfall_seq rainfall_pt

rainfall_seq: main_sequential.o utility.o
	$(CC) $(CFLAGS) -o rainfall_seq main_sequential.o utility.o -lpthread

rainfall_pt: main_parallel.o utility.o
	$(CC) $(CFLAGS) -o rainfall_pt main_parallel.o utility.o -lpthread

main_sequential.o: main_sequential.cpp Grid.h Node.h utility.h
	$(CC) $(CFLAGS) -c main_sequential.cpp

main_parallel.o: main_parallel.cpp Grid.h Node.h utility.h
	$(CC) $(CFLAGS) -c main_parallel.cpp

utility.o: utility.cpp utility.h
	$(CC) $(CFLAGS) -c utility.cpp

.PHONY:clean

clean:
	rm -f rainfall *.o
