LDLIBS+=-lm

fmcore: fmcore.o fmosc.o fmamp.o

.PHONY: clean
clean:
	rm -f *.o
