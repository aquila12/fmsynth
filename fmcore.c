// fmcore.c
// A minimal FM synthesizer

#include "stdlib.h"
#include "fmcore.h"
#include "fmfreq.h"
#include "fmosc.h"
#include "fmamp.h"
#include "fmadhr.h"

void fmel_update(fmel_t *el) {}
void fmel_event(fmel_t *el, fmevent_t event) {}
void fmel_cleanup(fmel_t *el) {}

int fmel_init(fmel_t *el) {
  el->update = fmel_update;
  el->event = fmel_event;
  el->cleanup = fmel_cleanup;
  el->out = 0;
  return 0;
}

void fmsub_update(fmel_t *el) {
  fmsub_t *co = (fmsub_t*)el;
  fmel_t **sel = co->p_elements;
  if(!sel) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(sel[i]) sel[i]->update(sel[i]);
  }
  co->el.out = sel[co->n_elements - 1]->out;
}

void fmsub_event(fmel_t *el, fmevent_t event) {
  fmsub_t *co = (fmsub_t*)el;
  fmel_t **sel = co->p_elements;
  if(!sel) return;

  for(int i=0; i<co->n_elements; ++i) {
    if(sel[i]) sel[i]->event(sel[i], event);
  }
}

void fmsub_cleanup(fmel_t *el) {
  fmsub_t *co = (fmsub_t*)el;

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

int fmsub_init(fmsub_t *co, size_t n_elements) {
  fmel_t *el = &co->el;
  if(fmel_init(el)) return -1;

  el->update  = fmsub_update;
  el->event   = fmsub_event;
  el->cleanup = fmsub_cleanup;

  co->p_elements = calloc(n_elements, sizeof(fmel_t*));
  co->n_elements = n_elements;
  return (co->p_elements ? 0 : -1);
}

int fmch_init(fmsub_t *ch /*, patch */, sample_t *lfo_out) {
  /* Patch specifies these */
  size_t n_freq = 1;
  size_t n_ops = 2;
  size_t n_adhr = 1;
  size_t n_ampl = 1;

  if(fmsub_init(ch, 1 + n_ops + n_adhr + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = ch->p_elements;
  fmfreq_t *freq = calloc(1, sizeof(fmfreq_t));
  fmfreq_init(freq);
  fmosc_t *op0 = calloc(1, sizeof(fmosc_t));
  fmosc_t *op1 = calloc(1, sizeof(fmosc_t));
  fmamp_t *amp = calloc(1, sizeof(fmamp_t));
  fmadhr_t *adhr = calloc(1, sizeof(fmadhr_t));
  fmosc_init(op0, 2.0, 0.006, &freq->el.f, lfo_out);
  fmosc_init(op1, 1.0, 0.8, &freq->el.f, &op0->el.out);
  fmamp_init(amp, 1);
  fmadhr_init(adhr, 20.0, 10.0, 0.2, 0.7, 3.0, &amp->el.out);
  fmamp_connect(amp, 0, &op1->el.out, 1.0);
  el[0] = &freq->el;
  el[1] = &op0->el;
  el[2] = &op1->el;
  // Configure ADSR
  el[3] = &amp->el;
  el[4] = &adhr->el;

  return 0;
}

int fminstr_init(fmsub_t *instr, size_t n_channels /*, patch */) {
  size_t n_el = 1; /* LFO frequency */
  size_t n_osc = 1; /* "Global" LFOs */
  size_t n_ampl = 1; /* Channel mixer */
  int n=0;

  if(fmsub_init(instr, n_el + n_osc + n_channels + n_ampl)) return -1;

  /* Patch does this */
  fmel_t *lfo_f = calloc(1, sizeof(fmel_t));
  fmel_init(lfo_f);
  lfo_f->f = 1.5 * HERTZ;

  fmosc_t *osc = calloc(1, sizeof(fmosc_t));
  fmosc_init(osc, 1.0, 0.0, &lfo_f->f, 0);

  fmel_t **el = instr->p_elements;
  el[n++] = lfo_f;
  el[n++] = &osc->el;
  fmamp_t *amp = calloc(1, sizeof(fmamp_t));
  if(fmamp_init(amp, n_channels)) return -1;

  fmsub_t *ch;
  for(int i=0; i<n_channels; ++i) {
    ch = calloc(1, sizeof(fmsub_t));
    if(fmch_init(ch, &osc->el.out)) return -1;
    fmamp_connect(amp, i, &ch->el.out, 1.0);
    el[n++] = &ch->el;
  }
  el[n++] = &amp->el;

  return 0;
}
