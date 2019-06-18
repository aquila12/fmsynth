// fmfreq.c
// Frequency converter

#include "fmfreq.h"
#include "math.h"

frequency_t freq_buf[FREQ_BUFFERSIZE];

void fmfreq_setup() {
  double phase;
  double k = 128.0 / 12.0 / (double)FREQ_BUFFERSIZE;
  double f0 = (double)FREQ_STANDARD / exp2((double)NOTE_STANDARD / (double)12) * HERTZ;

  for(int i=0; i<FREQ_BUFFERSIZE; ++i) {
    freq_buf[i] = f0 * exp2(k * (double)i);
  }
}

frequency_t frequency(note_t note) {
  int n = note >> (NOTE_FRACTION - FREQ_FRACTIONBITS);
  return freq_buf[n];
}

// void fmfreq_update(fmel_t *el) {
//   fmfreq_t *fq=(fmfreq_t*)el;
// }

void fmfreq_event(fmel_t *el, fmevent_t event) {
  fmfreq_t *fq=(fmfreq_t*)el;

  switch(event.type) {
    case fmev_note_on:
    fq->note = event.note_number << NOTE_FRACTION;
    fq->el.f = frequency(fq->note);
    break;
  }
}

void fmfreq_init(fmfreq_t *fq) {
  fmel_t *el = &fq->el;
  fmel_init(el);

  //el->update = fmfreq_update;
  el->event = fmfreq_event;

  fq->note = 0;
}
