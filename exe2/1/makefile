CC=clang++
CFLAGS= -Wall -g -fPIC -fprofile-arcs -ftest-coverage



all: ttt

ttt: ttt.o
	$(CC) $(CFLAGS) -o ttt ttt.o

ttt.o: ttt.cpp
	$(CC) $(CFLAGS) -c ttt.cpp -o ttt.o 

clean:
	rm -f ttt *.o *.info *.gcno *.gcda *.gcov
