// fmcore.c
// A minimal FM synthesizer

#include "stdint.h"
#include "stdio.h"
#include "unistd.h"
#include "math.h"

#define RATE 48000

#define PHASE_BITS 32
#define SINE_BUFBITS 15
#define PHASE_SIG_BITS (SINE_BUFBITS + 2)
#define PHASE_SINE_SHIFT (PHASE_BITS - PHASE_SIG_BITS)

#define SINE_BUFFERSIZE (1 << SINE_BUFBITS)
#define SINE_AMPLITUDE  INT32_MAX

typedef int32_t sample_t;
typedef uint32_t phase_t;
typedef int32_t frequency_t;

typedef struct fm_op_s {
  sample_t last_output;
} fm_op;

sample_t fm_output;
sample_t fm_sine_buf[SINE_BUFFERSIZE];

void fm_init() {
  double phase;
  double k = 3.14159265359 / (double)(2*SINE_BUFFERSIZE);
  double a = (double) SINE_AMPLITUDE;

  for(int i=0; i<SINE_BUFFERSIZE; ++i) {
    fm_sine_buf[i] = a * sin(k * (double)i);
  }
}

sample_t fm_sine(phase_t theta) {
  int p = (theta >> PHASE_SINE_SHIFT);
  int q = p >> SINE_BUFBITS;
  int r = p & (SINE_BUFFERSIZE - 1);

  int i = (q & 1) ? SINE_BUFFERSIZE - r - 1 : r;
  sample_t s = fm_sine_buf[i];

  return (q & 2) ? -s : s;
}

void fm_timestep() {
  static phase_t p=0;
  p += 39370534;
  fm_output = fm_sine(p);
}

int main() {
  fprintf(stderr, "Initializing\n");
  fm_init();
  fprintf(stderr, "Initialized\n");

  for(int i=0; i<RATE; ++i) {
    fm_timestep();
    int16_t s = fm_output >> 16;
    write(1, &s, sizeof(s));
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
