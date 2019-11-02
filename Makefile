LDLIBS+=-lm

CFLAGS+=-O3

fmtest: fmfunc.o fmop.o fmpatch.o fmrender.o fmcmd.o

fmtest.dbg: CFLAGS=-g
fmtest.dbg: CC=gcc-8
fmtest.dbg: fmtest

.PHONY: clean
clean:
	rm -f *.o fmtest
