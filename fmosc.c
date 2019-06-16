// fmcore.c
// A minimal FM synthesizer

#include "fmosc.h"
#include "math.h"

sample_t sine_buf[SINE_BUFFERSIZE];

void fmosc_init() {
  double phase;
  double k = 3.14159265359 / (double)(2*SINE_BUFFERSIZE);
  double a = (double) SINE_AMPLITUDE;

  for(int i=0; i<SINE_BUFFERSIZE; ++i) {
    sine_buf[i] = a * sin(k * (double)i);
  }
}

sample_t sine(phase_t theta) {
  int p = (theta >> PHASE_SINE_SHIFT);
  int q = p >> SINE_BUFBITS;
  int r = p & (SINE_BUFFERSIZE - 1);

  int i = (q & 1) ? SINE_BUFFERSIZE - r - 1 : r;
  sample_t s = sine_buf[i];

  return (q & 2) ? -s : s;
}

void fmosc_set(fmosc *osc, float freq_hz) {
  osc->f = freq_hz * HERTZ;
}

sample_t fmosc_update(fmel *el) {
  fmosc *osc = (fmosc*)el;
  osc->p += osc->f;
  return sine(osc->p);
}

void fmosc_configure(fmosc *osc, size_t count) {
  for(int i=0; i<count; ++i) {
    osc[i].el.update = fmosc_update;
  }
}
