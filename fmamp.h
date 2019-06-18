// fmamp.h
// Summing Amplifier

#include "fmcore.h"
#include "stddef.h"

/* TODO: Add AM modulation input */

typedef struct fmamp_s {
  fmel_t el;
  sample_t **inputs;
  sample_t *ampl;
  size_t n_inputs;
} fmamp_t;

int fmamp_init(fmamp_t *amp, size_t n_inputs);
void fmamp_connect(fmamp_t *amp, int index, sample_t *input, float amplitude);
void fmamp_set_amplitude(fmamp_t *amp, int index, float amplitude);
