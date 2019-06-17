// fmcore.c
// A minimal FM synthesizer

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"
#include "fmamp.h"

sample_t fm_output;

fmosc_t lfo_osc;

void fmel_update(fmel_t *el) {}
void fmel_event(fmel_t *el, fmevent_t event, const void *event_data) {}
void fmel_cleanup(fmel_t *el) {}

int fmel_init(fmel_t *el) {
  el->update = fmel_update;
  el->event = fmel_event;
  el->cleanup = fmel_cleanup;
  el->out = 0;
  return 0;
}

void fmcontainer_update(fmcontainer_t *co) {
  fmel_t **el = co->p_elements;
  if(!el) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(el[i]) el[i]->update(el[i]);
  }
  co->el.out = el[co->n_elements - 1]->out;
}

void fmcontainer_event(fmcontainer_t *co, fmevent_t event, const void *event_data) {
  fmel_t **el = co->p_elements;
  if(!el) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(el[i]) el[i]->event(el[i], event, event_data);
  }
}

void fmcontainer_cleanup(fmel_t *el) {
  fmcontainer_t *co = (fmcontainer_t*)el;

  if(co->p_elements) {
    for(int i=0; i<co->n_elements; ++i) {
      co->p_elements[i]->cleanup(co->p_elements[i]);
      free(co->p_elements[i]);
    }
    free(co->p_elements);
  }

  co->n_elements = 0;
  co->p_elements = 0;
}

int fmcontainer_init(fmcontainer_t *co, size_t n_elements) {
  fmel_t *el = &co->el;
  if(fmel_init(el)) return -1;

  el->update  = fmcontainer_update;
  el->event   = fmcontainer_event;
  el->cleanup = fmcontainer_cleanup;

  co->p_elements = calloc(n_elements, sizeof(fmel_t*));
  co->n_elements = n_elements;
  return (co->p_elements ? 0 : -1);
}

int fmch_init(fmcontainer_t *ch /*, patch */) {
  /* Patch specifies these */
  size_t n_ops = 2;
  size_t n_adsr = 0; //1;
  size_t n_ampl = 1;

  if(fmcontainer_init(ch, n_ops + n_adsr + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = ch->p_elements;
  fmosc_t *op0 = calloc(1, sizeof(fmosc_t));
  fmosc_t *op1 = calloc(1, sizeof(fmosc_t));
  fmamp_t *amp = calloc(1, sizeof(fmamp_t));
  fmosc_configure(op0, 2.0, 0.006, &lfo_osc.el);
  fmosc_configure(op1, 1.0, 0.4, &op0->el);
  fmamp_init(amp, 1);
  fmamp_connect(amp, 0, &op1->el, SAMPLE_1);
  el[0] = &op0->el;
  el[1] = &op1->el;
  // Configure ADSR
  el[2] = &amp->el;

  return 0;
}

int fminstr_init(fmcontainer_t *instr, size_t n_channels /*, patch */) {
  size_t n_osc = 0; /* "Global" LFOs */
  size_t n_ampl = 1; /* Channel mixer */

  if(fmcontainer_init(instr, n_osc + n_channels + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = instr->p_elements;
  fmamp_t *amp = calloc(n_ampl, sizeof(fmamp_t));
  fmamp_init(amp, n_channels);
  //fmosc_configure(...);
  for(int i=0; i<n_channels; ++i) {
    el[i] = calloc(1, sizeof(fmcontainer_t));
    fmch_init(el[i]);
    fmamp_connect(amp, i, el[i], SAMPLE_1);
  }
  el[n_channels] = &amp->el;

  return 0;
}

void render(float duration, fmel_t *root) {
  int n = RATE * duration;
  for(int i=0; i<n; ++i) {
    root->update(root);
    int16_t s = (root->out * 32) >> 16;
    write(1, &s, sizeof(s));
  }
}

int main() {
  fmcontainer_t *clar = calloc(1, sizeof(fmcontainer_t));
  fmel_t *root = &clar->el;
  float lfo_rate = 10.0;

  fprintf(stderr, "Initializing\n");
  fmosc_init();

  // Initialize 3 channels
  fminstr_init(clar, 3);

  fmosc_configure(&lfo_osc, 1.0, 0.0, 0);
  lfo_osc.el.event(&lfo_osc, fmev_freq_change, &lfo_rate);
  lfo_osc.el.event(&lfo_osc, fmev_note_on, 0);
  fprintf(stderr, "Initialized\n");

  const float c4 = 261.6256;
  const float e4 = 329.6276;
  const float g4 = 391.9954;

  for(int i=0; i<16; ++i) {
    clar->el.event(&clar->el, fmev_note_on, 0);
    clar->p_elements[0]->event(clar->p_elements[0], fmev_freq_change, &c4);
    clar->p_elements[1]->event(clar->p_elements[1], fmev_freq_change, &e4);
    clar->p_elements[2]->event(clar->p_elements[2], fmev_freq_change, &g4);
    render(0.9 * 0.25, root);
    clar->el.event(&clar->el, fmev_note_off, 0);
    render(0.1 * 0.25, root);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
