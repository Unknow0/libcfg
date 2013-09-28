PROG=libcfg
CC=gcc
CFLAGS=-g
LDFLAGS=-ljson -lcontainer
PREFIX=/usr/local

SRC=$(filter-out main.c, $(wildcard *.c))
OBJECTS=$(SRC:.c=.o)
INCLUDE=$(wildcard *.h)

all: build test

build: $(PROG).a $(PROG).so

$(PROG).a: $(OBJECTS)
	ar rcs $(PROG).a $^
$(PROG).so: $(OBJECTS)
	$(CC) -shared -o $(PROG).so $^

test: main.c $(PROG).a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o test

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean install uninstall

clean:
	rm $(OBJECTS)
	rm test

install: build
	cp $(PROG)* $(PREFIX)/lib/
	cp $(INCLUDE) $(PREFIX)/include/

uninstall:
	rm $(PREFIX)/lib/$(PROG)*
	rm -r $(PREFIX)/include/$(INCLUDE)
