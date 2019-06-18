// fmadhr.c
// Attack-Decay-Hold-Release envelope unit

#include "fmadhr.h"
#include "stdlib.h"

void fmadhr_update(fmel_t *el) {
  fmadhr_t *adhr = (fmadhr_t*)el;

  adhr->ampl += adhr->rate[adhr->mode];

  if(adhr->ampl <= 0) adhr->mode = adhr_idle;
  if(adhr->mode == adhr_idle) el->out = 0;
  else {
    if(adhr->mode == adhr_attack && adhr->ampl >= SAMPLE_1) {
      adhr->ampl = SAMPLE_1;
      adhr->mode = adhr_decay;
    }

    if(adhr->mode == adhr_decay && adhr->ampl <= adhr->h_level) {
      adhr->ampl = adhr->h_level;
      adhr->mode = adhr_hold;
    }

    el->out = MUL(*adhr->input, adhr->ampl);
  }
}

void fmadhr_event(fmel_t *el, fmevent_t event, const void *event_data) {
  fmadhr_t *adhr = (fmadhr_t*)el;

  switch(event) {
    case fmev_note_on:
    adhr->mode = adhr_attack;
    break;

    case fmev_note_off:
    adhr->mode = adhr_release;
  }
}

int fmadhr_init(fmadhr_t *adhr, float a_rate, float d_rate, float h_rate, float h_level, float r_rate, sample_t *input) {
  fmel_t *el = &adhr->el;
  fmel_init(el);

  el->update = fmadhr_update;
  el->event = fmadhr_event;

  adhr->input = input;
  adhr->ampl = 0;
  adhr->rate[adhr_idle] = 0;
  adhr->rate[adhr_attack]  = a_rate * ADHR_RATE_CONSTANT;
  adhr->rate[adhr_decay]   = -d_rate * ADHR_RATE_CONSTANT;
  adhr->rate[adhr_hold]    = -h_rate * ADHR_RATE_CONSTANT;
  adhr->rate[adhr_release] = -r_rate * ADHR_RATE_CONSTANT;
  adhr->h_level = h_level * SAMPLE_1;
  adhr->mode = adhr_idle;

  return 0;
}
