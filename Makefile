CFLAGS ?= -Wall -g
LDFLAGS ?=
LIBDIR ?= /usr/lib

AR = ar
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all tests tools test check check-plan js-test js-check-plan benchmark sanitize ci install clean deb

all: libitute164.a tests tools

tests: libitute164.a
	$(MAKE) -C tests

tools: libitute164.a
	$(MAKE) -C tools

check: all
	./tests/tests
	$(MAKE) -C tools check
	$(MAKE) check-plan

test: check

check-plan: tools
	tools/plan-check data/e164-plan.txt

js-test:
	cd js && npm test

js-check-plan:
	cd js && npm run check-plan

benchmark: libitute164.a
	$(MAKE) -C tools benchmark

sanitize:
	$(MAKE) clean
	ASAN_OPTIONS=detect_leaks=0 $(MAKE) CFLAGS='-Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -fno-omit-frame-pointer' LDFLAGS='-fsanitize=address,undefined' check

ci:
	$(MAKE) clean
	$(MAKE) check
	$(MAKE) sanitize
	$(MAKE) clean
	rm -rf /tmp/libitute164-ci-install
	$(MAKE) install DESTDIR=/tmp/libitute164-ci-install
	/tmp/libitute164-ci-install/usr/bin/itute164-plan-check /tmp/libitute164-ci-install/usr/share/libitute164/e164-plan.txt
	rm -rf /tmp/libitute164-ci-install

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ -o $@

libitute164.a: $(OBJECTS)
	$(AR) rcs $@ $^

install: libitute164.a tools
	install -d $(DESTDIR)/usr/include
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)/usr/share/libitute164
	install -d $(DESTDIR)/usr/bin
	install -m 644 libitute164.h $(DESTDIR)/usr/include
	install -m 644 libitute164.a $(DESTDIR)$(LIBDIR)/
	install -m 644 data/e164-plan.txt $(DESTDIR)/usr/share/libitute164/
	install -m 755 tools/plan-check $(DESTDIR)/usr/bin/itute164-plan-check

clean:
	$(MAKE) -C tests clean
	$(MAKE) -C tools clean
	rm -f *.o *.a

deb:
	dpkg-buildpackage -b
