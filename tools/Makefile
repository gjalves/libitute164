CFLAGS=-Wall -g -Ilib/ -I..

all: input

input: input.o ../libitute164.a
	$(CC) $(CFLAGS) -Llib -o $@ $^ -lncurses

clean:
	rm -f *.o
