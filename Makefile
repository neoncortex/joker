CC=gcc
CCARGS=-Wall
PROGNAME=joker
IPATH=/usr/local/bin

default:$(PROGNAME).c
	$(CC) $(CCARGS) $(PROGNAME).c -o $(PROGNAME)

install:$(PROGNAME)
	cp $(PROGNAME) $(IPATH)

clean:
	rm $(PROGNAME)
