// fmosc.c
// FM Oscillator

#include "fmosc.h"
#include "math.h"
#include "stdlib.h"

sample_t sine_buf[SINE_BUFFERSIZE];

void fmosc_setup() {
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

void fmosc_update(fmel_t *el) {
  fmosc_t *osc = (fmosc_t*)el;
  sample_t mod_input;
  frequency_t freq_input;

  if(!osc->f0) return;
  freq_input = MUL(*osc->f0, osc->fmul);

  if(osc->beta && osc->input && osc->f0) {
    mod_input = SAMPLE_1 + MUL(osc->beta, *osc->input);
    osc->p += MUL(freq_input, mod_input);
  } else if(osc->f0) {
    osc->p += freq_input;
  }

  el->out = sine(osc->p);
}

void fmosc_init(fmosc_t *osc, float freq_mul, float mod_index, frequency_t *f0, sample_t *input) {
  fmel_t *el = &osc->el;
  fmel_init(el);

  el->update = fmosc_update;

  osc->p = 0;
  osc->f0 = f0;
  osc->fmul = freq_mul * SAMPLE_1;
  osc->beta = mod_index * SAMPLE_1;
  osc->input = input;
}
