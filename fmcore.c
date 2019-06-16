// fmcore.c
// A minimal FM synthesizer

#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"

/* Determines whether calculation is stale */
int8_t sample_number = 0;

sample_t fm_output;

#define FM_OPS 3
fmosc fm_operators[FM_OPS];

sample_t fmel_resolve(fmel *el) {
  if((int8_t)(el->last_updated - sample_number) < 0) {
    el->last_updated = sample_number;
    el->out = el->update(el);
  }
  return el->out;
}

void fmel_event(fmel *el, fmevent_t event, const void *event_data) {
  el->event(el, event, event_data);
}

void fm_timestep() {
  sample_number += 1;

  fm_output = 0;
  for(int i=0; i<FM_OPS; ++i) {
    fmel_resolve(&fm_operators[i].el);
    fm_output += fm_operators[i].el.out / 8;
  }
}

void render(float duration) {
  int n = RATE * duration;
  for(int i=0; i<n; ++i) {
    fm_timestep();
    int16_t s = fm_output >> 16;
    write(1, &s, sizeof(s));
  }
}

int main() {
  fprintf(stderr, "Initializing\n");
  fmosc_init();
  fmosc_configure(fm_operators, FM_OPS);
  fprintf(stderr, "Initialized\n");

  const float c4 = 261.6256;
  const float e4 = 329.6276;
  const float g4 = 391.9954;

  for(int i=0; i<8; ++i) {
    fmel_event(&fm_operators[0].el, fmev_note_on, 0);
    fmel_event(&fm_operators[1].el, fmev_note_on, 0);
    fmel_event(&fm_operators[2].el, fmev_note_on, 0);
    fmel_event(&fm_operators[0].el, fmev_freq_change, &c4);
    fmel_event(&fm_operators[1].el, fmev_freq_change, &e4);
    fmel_event(&fm_operators[2].el, fmev_freq_change, &g4);
    render(0.9 * 0.25);
    fmel_event(&fm_operators[0].el, fmev_note_off, 0);
    fmel_event(&fm_operators[1].el, fmev_note_off, 0);
    fmel_event(&fm_operators[2].el, fmev_note_off, 0);
    render(0.1 * 0.25);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
