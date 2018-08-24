CC=gcc
CFLAGS=-DNDEBUG -O3 -Wall -Wno-unused-result -std=c11 -pedantic
# debug flags
CDFLAGS=-ggdb -Wall -Wno-unused-result -std=c11 -pedantic
LDFLAGS=

all: linja

linja: linja.o move.o board.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf *.o && rm linja

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
