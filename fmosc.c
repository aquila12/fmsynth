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

sample_t fmosc_update(fmel *el) {
  fmosc *osc = (fmosc*)el;
  int32_t mod_input;

  if(osc->beta && osc->input) {
    mod_input = SAMPLE_1 + MUL(osc->beta, fmel_resolve(osc->input));
    osc->p += MUL(osc->f0, mod_input);
  } else {
    osc->p += osc->f0;
  }

  return sine(osc->p);
}

void fmosc_event(fmel *el, fmevent_t event, const void *event_data) {
  fmosc *osc = (fmosc*)el;
  switch(event) {
    case fmev_freq_change:
    osc->f0 = *((float*)event_data) * osc->fmul * HERTZ;
    break;
  }
}

void fmosc_configure(fmosc *osc, float freq_mul, float mod_index, fmel *input) {
  osc->el.update = fmosc_update;
  osc->el.event = fmosc_event;

  osc->p = 0;
  osc->f0 = 0;
  osc->fmul = freq_mul;
  osc->beta = mod_index * SAMPLE_1;
  osc->input = input;
}
