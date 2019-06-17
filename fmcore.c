// fmcore.c
// A minimal FM synthesizer

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"

/* Determines whether calculation is stale */
int8_t sample_number = 0;

sample_t fm_output;

fmel_t lfo_el;
fmosc_t lfo_osc;

int fminstr_init_ch(fmel_t **pel) {
  /* Patch does this */
  pel[0] = calloc(1, sizeof(fmosc_t));
  pel[1] = calloc(1, sizeof(fmosc_t));
  fmosc_configure(pel[0], 2.0, 0.006, &lfo_el);
  fmosc_configure(pel[1], 1.0, 0.4, pel[0]);
  // Configure ADSR
  // Configure Amplifier
}

int fminstr_init(fminstr_t *instr, /* patch, */ size_t n_channels) {
  instr->channels = malloc(n_channels * sizeof(fminstr_t));
  instr->n_channels = 1;

  /* Patch specifies these */
  size_t n_ops = 2;
  size_t n_adsr = 1;
  size_t n_ampl = 1;

  size_t n_elements = n_channels * (n_ops + n_adsr + n_ampl);

  /* FIXME: Should not be doing calloc, needs to be sized correctly */
  instr->elements = calloc(n_elements, sizeof(fmel_t*));
  instr->n_elements = n_elements;

  if(instr->elements==0 || instr->channels==0) return -1;

  for(int i=0; i<n_channels; ++i) {
    size_t els_per_ch = (n_ops + n_adsr + n_ampl);
    fminstr_init_ch(&instr->elements[i * els_per_ch]);
  }

  return 0;
}

void fminstr_free(fminstr_t *instr) {
  if(instr->channels) free(instr->channels);
  if(instr->elements) free(instr->elements);

  instr->channels = 0;
  instr->elements = 0;
  instr->n_channels = instr->n_elements = 0;
}

void fminstr_timestep(fminstr_t *instr) {
  fmel_t *el;
  for(int i=0; i<instr->n_elements; ++i) {
    el = instr->elements[i];
    if(el && el->update) el->update(el);
  }
}

void fminstr_event(fminstr_t *instr, fmevent_t event, const void *event_data) {
  fmel_t *el;
  for(int i=0; i<instr->n_elements; ++i) {
    el = instr->elements[i];
    if(el && el->event) el->event(el, event, event_data);
  }
}

void render(float duration, fminstr_t *instr) {
  int n = RATE * duration;
  for(int i=0; i<n; ++i) {
    fminstr_timestep(instr);
    int16_t s = (instr->elements[1]->out * 32) >> 16;
    write(1, &s, sizeof(s));
  }
}

int main() {
  fminstr_t clar;
  float lfo_rate = 10.0;

  fprintf(stderr, "Initializing\n");
  fmosc_init();

  // Initialize 3 channels
  fminstr_init(&clar, 3);

  fmosc_configure(&lfo_el, 1.0, 0.0, 0);
  lfo_el.event(&lfo_el, fmev_freq_change, &lfo_rate);
  lfo_el.event(&lfo_el, fmev_note_on, 0);
  fprintf(stderr, "Initialized\n");

  const float c4 = 261.6256;
  const float e4 = 329.6276;
  const float g4 = 391.9954;

  for(int i=0; i<16; ++i) {
    fminstr_event(&clar, fmev_note_on, 0);
    fminstr_event(&clar, fmev_freq_change, &c4);
    //fmel_event(clar->[1], fmev_freq_change, &e4);
    //fmel_event(clar->[2], fmev_freq_change, &g4);
    render(0.9 * 0.25, &clar);
    fminstr_event(&clar, fmev_note_off, 0);
    //fmel_event(&fm_operators[1], fmev_note_off, 0);
    //fmel_event(&fm_operators[2], fmev_note_off, 0);
    render(0.1 * 0.25, &clar);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
