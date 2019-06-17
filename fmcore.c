// fmcore.c
// A minimal FM synthesizer

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"

sample_t fm_output;

fmosc_t lfo_osc;

int fmch_init(fmcontainer_t *ch /*, patch */) {
  /* Patch specifies these */
  size_t n_ops = 2;
  size_t n_adsr = 1;
  size_t n_ampl = 1;

  size_t n_elements = n_ops + n_adsr + n_ampl;

  if(fmcontainer_init(ch, n_ops + n_adsr + n_ampl)) return -1;

  /* Patch does this */
  fmel_t **el = ch->p_elements;
  fmosc_t* op = calloc(2, sizeof(fmosc_t)); /* FIXME: never freed */
  fmosc_configure(&op[0], 2.0, 0.006, &lfo_osc);
  fmosc_configure(&op[1], 1.0, 0.4, &op[0]);
  el[0] = &op[0];
  el[1] = &op[1];
  // Configure ADSR
  // Configure Amplifier

  return 0;
}

int fmcontainer_init(fmcontainer_t *co, size_t n_elements) {
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

void fmch_timestep(fmcontainer_t *ch) {
  fmel_t **el = ch->p_elements;
  if(!el) return;

  for(int i=0; i<ch->n_elements; ++i) {
    if(el[i] && el[i]->update) el[i]->update(el[i]);
  }
}

void fmch_event(fmcontainer_t *ch, fmevent_t event, const void *event_data) {
  fmel_t **el = ch->p_elements;
  if(!el) return;

  for(int i=0; i<ch->n_elements; ++i) {
    if(el[i] && el[i]->event) el[i]->event(el[i], event, event_data);
  }
}

void render(float duration, fmcontainer_t *ch) {
  int n = RATE * duration;
  for(int i=0; i<n; ++i) {
    fmch_timestep(ch);
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
    fmch_event(clar, fmev_note_on, 0);
    fmch_event(clar, fmev_freq_change, &c4);
    //fmel_event(clar->[1], fmev_freq_change, &e4);
    //fmel_event(clar->[2], fmev_freq_change, &g4);
    render(0.9 * 0.25, clar);
    fmch_event(clar, fmev_note_off, 0);
    //fmel_event(&fm_operators[1], fmev_note_off, 0);
    //fmel_event(&fm_operators[2], fmev_note_off, 0);
    render(0.1 * 0.25, clar);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
