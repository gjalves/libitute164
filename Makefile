CFLAGS=-Wall -g

AR = ar
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

all: libitute164.a

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

libitute164.a: $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	rm -f *.o *.a
