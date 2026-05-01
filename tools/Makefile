CFLAGS ?= -Wall -g
LDFLAGS ?=
CPPFLAGS = -Ilib/ -I..
LDLIBS = -lncurses

.PHONY: all benchmark clean

all: input plan-check

input: input.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -Llib -o $@ $^ $(LDLIBS)

plan-check: plan-check.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

bench-validate: bench-validate.o ../libitute164.a
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

benchmark: bench-validate
	./bench-validate

clean:
	rm -f *.o input plan-check bench-validate
