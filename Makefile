CC=gcc
CCARGS=-Wall -Wextra -ggdb
PROGNAME=joker
IPATH=/usr/local/bin

default:$(PROGNAME).c $(PROGNAME).h data.o
	$(CC) $(CCARGS) -o $(PROGNAME) data.o $(PROGNAME).c

data.o:data.c data.h
	$(CC) -c $(CCARGS) data.c

install:$(PROGNAME)
	cp $(PROGNAME) $(IPATH)

clean:
	rm data.o
	rm $(PROGNAME)

