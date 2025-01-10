CC=gcc
CFLAGS=-Wall -g
CLINK=$(CC)

prog.exe: linked-list.o hash-table.o main.o
	$(CLINK) linked-list.o hash-table.o main.o -o prog.exe 

linked-list.o: linked-list.h linked-list.c
hash-table.o: linked-list.h hash-table.h hash-table.c
main.o: main.c hash-table.h

clean:
	rm -f *.o *.exe