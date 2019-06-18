LDLIBS+=-lm

fmcore: fmcore.o fmosc.o fmamp.o fmadhr.o

.PHONY: clean
clean:
	rm -f *.o fmcore
