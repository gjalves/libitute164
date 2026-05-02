CFLAGS ?= -Wall -g
LDFLAGS ?=
CPPFLAGS = -Ilib/ -I..
LDLIBS = -lncurses

.PHONY: all benchmark check clean

all: input plan-check

input: input.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -Llib -o $@ $^ $(LDLIBS)

input-test: input-test.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -Llib -o $@ $^ $(LDLIBS)

plan-check: plan-check.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

bench-validate: bench-validate.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

benchmark: bench-validate
	./bench-validate

check: input-test
	./input-test

clean:
	rm -f *.o input input-test plan-check bench-validate
