CFLAGS=-Wall -g

AR = ar
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

all: libitute164.a
	make -C tests
	make -C tools

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

libitute164.a: $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	make -C tests clean
	make -C tools clean
	rm -f *.o *.a
