CFLAGS ?= -Wall -g
LDFLAGS ?=
CPPFLAGS = -Ilib/ -I..
LDLIBS = -lncurses

.PHONY: all clean

all: input plan-check

input: input.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -Llib -o $@ $^ $(LDLIBS)

plan-check: plan-check.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o input plan-check
