CC = gcc

CFLAGS = -g -Wall

copycat: copycat.o

copycat.o: copycat.c

.PHONY: clean
clean:
	rm -f *.o a.out copycat

leaks:
	leaks -atExit -- ./copycat copycat.c
