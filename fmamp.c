// fmamp.c
// Summing Amplifier

#include "fmamp.h"
#include "stdlib.h"

void fmamp_update(fmel_t *el) {
  fmamp_t *amp = (fmamp_t*)el;
  el->out = 0;

  for(int i=0; i<amp->n_inputs; ++i) {
    if(amp->inputs[i] && amp->ampl[i])
      el->out += MUL(amp->inputs[i]->out, amp->ampl[i]);
  }
}

void fmamp_cleanup(fmel_t *el) {
  fmamp_t *amp = (fmamp_t*) el;
  if(amp->inputs) free(amp->inputs);
  if(amp->ampl) free(amp->ampl);
  amp->inputs = 0;
  amp->ampl = 0;
}

int fmamp_init(fmamp_t *amp, size_t n_inputs) {
  fmel_t *el = &amp->el;
  fmel_init(el);

  el->update = fmamp_update;
  //el->event = fmosc_event;
  el->cleanup = fmamp_cleanup;

  amp->inputs = calloc(n_inputs, sizeof(fmel_t*));
  amp->ampl   = calloc(n_inputs, sizeof(sample_t));
  if(!amp->inputs || !amp->ampl) return -1;

  amp->n_inputs = n_inputs;

  return 0;
}

void fmamp_connect(fmamp_t *amp, int index, fmel_t *input, float amplitude) {
  if(index >= amp->n_inputs) return;
  amp->inputs[index] = input;
  amp->ampl[index] = amplitude * SAMPLE_1;
}

void fmamp_set_amplitude(fmamp_t *amp, int index, float amplitude) {
  if(index >= amp->n_inputs) return;
  amp->ampl[index] = amplitude * SAMPLE_1;
}
