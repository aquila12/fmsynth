LDLIBS+=-lm

fmtest: fmtest.o fmcore.o fmosc.o fmamp.o fmadhr.o fmfreq.o

.PHONY: clean
clean:
	rm -f *.o fmcore
