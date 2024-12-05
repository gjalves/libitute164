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

install:
	install -d $(DESTDIR)/usr/include
	install -d $(DESTDIR)/usr/lib
	install libitute164.h $(DESTDIR)/usr/include
	install libitute164.a $(DESTDIR)/usr/lib/

clean:
	make -C tests clean
	make -C tools clean
	rm -f *.o *.a

deb:
	dpkg-buildpackage -b
