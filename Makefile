LDLIBS+=-lm

fmcore: fmcore.o fmosc.o

.PHONY: clean
clean:
	rm -f *.o
