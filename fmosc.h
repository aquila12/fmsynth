// fmosc.h
// FM Oscillator

#include "fmcore.h"
#include "stddef.h"

#define PHASE_BITS 32
#define SINE_BUFBITS 15
#define PHASE_SIG_BITS (SINE_BUFBITS + 2)
#define PHASE_SINE_SHIFT (PHASE_BITS - PHASE_SIG_BITS)

#define SINE_BUFFERSIZE (1 << SINE_BUFBITS)
#define SINE_AMPLITUDE  INT32_MAX

#define HERTZ ((float)UINT32_MAX / (float)RATE)

typedef uint32_t phase_t;
typedef int32_t frequency_t;

typedef struct fmosc_s {
  fmel el;
  frequency_t f;
  phase_t p;
} fmosc;

void fmosc_init();
void fmosc_configure(fmosc *osc, size_t count);
void fmosc_set(fmosc *osc, float freq);
