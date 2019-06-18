// fmcore.c
// A minimal FM synthesizer

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"
#include "fmamp.h"
#include "fmadhr.h"

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

void fmcontainer_update(fmel_t *el) {
  fmcontainer_t *co = (fmcontainer_t*)el;
  fmel_t **sel = co->p_elements;
  if(!sel) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(sel[i]) sel[i]->update(sel[i]);
  }
  co->el.out = sel[co->n_elements - 1]->out;
}

void fmcontainer_event(fmel_t *el, fmevent_t event, const void *event_data) {
  fmcontainer_t *co = (fmcontainer_t*)el;
  fmel_t **sel = co->p_elements;
  if(!sel) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(sel[i]) sel[i]->event(sel[i], event, event_data);
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
  size_t n_adhr = 1;
  size_t n_ampl = 1;

  if(fmcontainer_init(ch, n_ops + n_adhr + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = ch->p_elements;
  fmosc_t *op0 = calloc(1, sizeof(fmosc_t));
  fmosc_t *op1 = calloc(1, sizeof(fmosc_t));
  fmamp_t *amp = calloc(1, sizeof(fmamp_t));
  fmadhr_t *adhr = calloc(1, sizeof(fmadhr_t));
  fmosc_init(op0, 2.0, 0.006, &lfo_osc.el.out);
  fmosc_init(op1, 1.0, 0.8, &op0->el.out);
  fmamp_init(amp, 1);
  fmadhr_init(adhr, 20.0, 10.0, 0.2, 0.7, 3.0, &amp->el.out);
  fmamp_connect(amp, 0, &op1->el.out, 1.0);
  el[0] = &op0->el;
  el[1] = &op1->el;
  // Configure ADSR
  el[2] = &amp->el;
  el[3] = &adhr->el;

  return 0;
}

int fminstr_init(fmcontainer_t *instr, size_t n_channels /*, patch */) {
  size_t n_osc = 0; /* "Global" LFOs */
  size_t n_ampl = 1; /* Channel mixer */

  if(fmcontainer_init(instr, n_osc + n_channels + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = instr->p_elements;
  fmamp_t *amp = calloc(n_ampl, sizeof(fmamp_t));
  if(fmamp_init(amp, n_channels)) return -1;

  fmcontainer_t *ch;
  for(int i=0; i<n_channels; ++i) {
    ch = calloc(1, sizeof(fmcontainer_t));
    if(fmch_init(ch)) return -1;
    el[i] = &ch->el;
    fmamp_connect(amp, i, &el[i]->out, 1.0);
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
  fmosc_setup();

  // Initialize 3 channels
  fminstr_init(clar, 3);

  fmosc_init(&lfo_osc, 1.0, 0.0, 0);
  lfo_osc.el.event(&lfo_osc.el, fmev_freq_change, &lfo_rate);
  lfo_osc.el.event(&lfo_osc.el, fmev_note_on, 0);
  fprintf(stderr, "Initialized\n");

  const float c4 = 261.6256;
  const float e4 = 329.6276;
  const float g4 = 391.9954;

  for(int i=0; i<3; ++i) {
    clar->el.event(&clar->el, fmev_note_on, 0);
    clar->p_elements[0]->event(clar->p_elements[0], fmev_freq_change, &c4);
    clar->p_elements[1]->event(clar->p_elements[1], fmev_freq_change, &e4);
    clar->p_elements[2]->event(clar->p_elements[2], fmev_freq_change, &g4);
    render(5.0 * 0.25, root);
    clar->el.event(&clar->el, fmev_note_off, 0);
    render(1.0 * 0.25, root);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
