CC = g++
CFLAGS = -O3 -std=c++11

rainfall: main.o simulation.o utility.o
	$(CC) $(CFLAGS) -o rainfall main.o simulation.o utility.o

main.o: main.cpp Grid.h Node.h utility.h
	$(CC) $(CFLAGS) -c main.cpp

simulation.o: simulation.cpp simulation.h utility.h
	$(CC) $(CFLAGS) -c simulation.cpp

utility.o: utility.cpp utility.h
	$(CC) $(CFLAGS) -c utility.cpp

.PHONY:clean

clean:
	rm -f rainfall *.o
