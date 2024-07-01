CC=clang++
CFLAGS= -Wall -g -fPIC -fprofile-arcs -ftest-coverage

all: ttt mync

mync: mync.o
	$(CC) $(CFLAGS) -o mync mync.o

mync.o: mync.cpp
	$(CC) $(CFLAGS) -c mync.cpp -o mync.o 

ttt: ttt.o
	$(CC) $(CFLAGS) -o ttt ttt.o

ttt.o: ttt.cpp
	$(CC) $(CFLAGS) -c ttt.cpp -o ttt.o 

clean:
	rm -f ttt mync *.o *.info *.gcno *.gcda *.gcov core.*