// fmosc.h
// FM Oscillator

#include "fmcore.h"
#include "stddef.h"

#define SINE_BUFBITS 15
#define PHASE_SIG_BITS (SINE_BUFBITS + 2)
#define PHASE_SINE_SHIFT (PHASE_BITS - PHASE_SIG_BITS)

#define SINE_BUFFERSIZE (1 << SINE_BUFBITS)
#define SINE_AMPLITUDE  SAMPLE_1

typedef struct fmosc_s {
  fmel_t el;
  frequency_t f0;
  phase_t p;
  float fmul;
  sample_t beta;
  sample_t *input;
} fmosc_t;

void fmosc_setup();
void fmosc_init(fmosc_t *osc, float freq_mul, float mod_index, sample_t *input);
