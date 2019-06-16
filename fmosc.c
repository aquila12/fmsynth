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
  if(osc->en) osc->p += osc->f;
  return sine(osc->p);
}

void fmosc_event(fmel *el, fmevent_t event, const void *event_data) {
  fmosc *osc = (fmosc*)el;
  switch(event) {
    case fmev_note_on:
    osc->en = 1;
    break;

    case fmev_note_off:
    osc->en = 0;
    break;

    case fmev_freq_change:
    osc->f = *((float*)event_data) * HERTZ;
    break;
  }
}

void fmosc_configure(fmosc *osc, size_t count) {
  for(int i=0; i<count; ++i) {
    osc[i].el.update = fmosc_update;
    osc[i].el.event = fmosc_event;
    osc[i].en = 0;
  }
}
