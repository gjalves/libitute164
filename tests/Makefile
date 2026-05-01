.PHONY: all clean

CFLAGS ?= -Wall -g
LDFLAGS ?=
CPPFLAGS = -I..
LDLIBS = -lcmocka

all: tests

tests: tests.c ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f tests
