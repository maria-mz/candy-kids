CC=cc
CXX=CC
CCFLAGS= -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wall -Werror -lrt

all: candykids

%.o : %.c
	$(CC) -c $(CCFLAGS) $<

candykids: candykids.o bbuff.o stats.o
	$(CC) -o candykids candykids.o bbuff.o stats.o -pthread

clean:
	rm -f core *.o candykids