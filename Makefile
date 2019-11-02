VPATH = src/

LDLIBS+=-lm

CFLAGS+=-O3

fmsynth: fmfunc.o fmop.o fmpatch.o fmrender.o fmcmd.o

fmsynth.dbg: CFLAGS=-g
fmsynth.dbg: CC=gcc-8
fmsynth.dbg: fmsynth

.PHONY: clean
clean:
	rm -f *.o fmsynth
