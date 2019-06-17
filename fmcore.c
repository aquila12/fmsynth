// fmcore.c
// A minimal FM synthesizer

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"

sample_t fm_output;

fmosc_t lfo_osc;

int fmel_init(fmel_t *el) {
  el->update = 0;
  el->event = 0;
  el->out = 0;
  return 0;
}

void fmcontainer_update(fmcontainer_t *co);
void fmcontainer_event(fmcontainer_t *co, fmevent_t event, const void *event_data);
int fmcontainer_init(fmcontainer_t *co, size_t n_elements) {
  if(fmel_init(co)) return -1;

  co->el.update = fmcontainer_update;
  co->el.event  = fmcontainer_event;

  if(co->p_elements) free(co->p_elements);
  co->p_elements = calloc(n_elements, sizeof(fmel_t*));
  co->n_elements = n_elements;
  return (co->p_elements ? 0 : -1);
}

/* NB: Free your own elements */
void fmcontainer_free(fmcontainer_t *co) {
  if(co->p_elements) free(co->p_elements);

  co->n_elements = 0;
  co->p_elements = 0;
}

void fmcontainer_update(fmcontainer_t *co) {
  fmel_t **el = co->p_elements;
  if(!el) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(el[i] && el[i]->update) el[i]->update(el[i]);
  }
}

void fmcontainer_event(fmcontainer_t *co, fmevent_t event, const void *event_data) {
  fmel_t **el = co->p_elements;
  if(!el) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(el[i] && el[i]->event) el[i]->event(el[i], event, event_data);
  }
}

int fmch_init(fmcontainer_t *ch /*, patch */) {
  /* Patch specifies these */
  size_t n_ops = 2;
  size_t n_adsr = 1;
  size_t n_ampl = 1;

  if(fmcontainer_init(ch, n_ops + n_adsr + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = ch->p_elements;
  fmosc_t *op = calloc(2, sizeof(fmosc_t)); /* FIXME: never freed */
  fmosc_configure(&op[0], 2.0, 0.006, &lfo_osc);
  fmosc_configure(&op[1], 1.0, 0.4, &op[0]);
  el[0] = &op[0];
  el[1] = &op[1];
  // Configure ADSR
  // Configure Amplifier

  return 0;
}

int fminstr_init(fmcontainer_t *instr, size_t n_channels /*, patch */) {
  size_t n_osc = 0; /* "Global" LFOs */
  size_t n_ampl = 1; /* Channel mixer */

  if(fmcontainer_init(instr, n_osc + n_channels + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = instr->p_elements;
  fmosc_t *op = 0;
  fmcontainer_t *ch = calloc(n_channels, sizeof(fmcontainer_t)); /* FIXME: never freed */
  //fmosc_configure(...);
  for(int i=0; i<n_channels; ++i) {
    fmch_init(&ch[i]);
    el[i] = &ch[i];
  }
  //fmamp_configure(...);

  return 0;
}

void render(float duration, fmcontainer_t *ch) {
  int n = RATE * duration;
  for(int i=0; i<n; ++i) {
    if(ch && ch->el.update) ch->el.update(ch);
    int16_t s = (ch->p_elements[1]->out * 32) >> 16;
    write(1, &s, sizeof(s));
  }
}

int main() {
  fmcontainer_t *clar = calloc(1, sizeof(fmcontainer_t));
  float lfo_rate = 10.0;

  fprintf(stderr, "Initializing\n");
  fmosc_init();

  // Initialize 3 channels
  fmch_init(clar);

  fmosc_configure(&lfo_osc, 1.0, 0.0, 0);
  lfo_osc.el.event(&lfo_osc, fmev_freq_change, &lfo_rate);
  lfo_osc.el.event(&lfo_osc, fmev_note_on, 0);
  fprintf(stderr, "Initialized\n");

  const float c4 = 261.6256;
  const float e4 = 329.6276;
  const float g4 = 391.9954;

  for(int i=0; i<16; ++i) {
    fmcontainer_event(clar, fmev_note_on, 0);
    fmcontainer_event(clar, fmev_freq_change, &c4);
    //fmel_event(clar->[1], fmev_freq_change, &e4);
    //fmel_event(clar->[2], fmev_freq_change, &g4);
    render(0.9 * 0.25, clar);
    fmcontainer_event(clar, fmev_note_off, 0);
    //fmel_event(&fm_operators[1], fmev_note_off, 0);
    //fmel_event(&fm_operators[2], fmev_note_off, 0);
    render(0.1 * 0.25, clar);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
