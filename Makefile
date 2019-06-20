LDLIBS+=-lm

CFLAGS+=-O3

fmtest: fmtest.o fmcore.o fmosc.o fmamp.o fmadhr.o fmfreq.o fminstr.o

fmtest.dbg: CFLAGS=-g
fmtest.dbg: CC=gcc-8
fmtest.dbg: fmtest

.PHONY: clean
clean:
	rm -f *.o fmcore
