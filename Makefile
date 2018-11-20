CC = g++
CFLAGS = -O3 -std=c++11

rainfall: main.o utility.o
	$(CC) $(CFLAGS) -o rainfall main.o utility.o

main.o: main.cpp Grid.h Node.h utility.h
	$(CC) $(CFLAGS) -c main.cpp

utility.o: utility.cpp utility.h
	$(CC) $(CFLAGS) -c utility.cpp

.PHONY:clean

clean:
	rm -f rainfall *.o
