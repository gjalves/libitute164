all: tests

tests: tests.c ../libitute164.a
	$(CC) -o $@ -Wall $^ ../libitute164.a  -I.. -lcmocka

clean:
	rm -f tests
