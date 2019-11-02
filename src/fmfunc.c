// fmfunc.c
// FM Maths Functions
// Two plus two is four minus one dat's free quick maffs

#include "fmfunc.h"
#include "math.h"

sample_t sine_buf[SINE_BUFFERSIZE];
frequency_t freq_buf[FREQ_BUFFERSIZE];

void sine_setup() {
  double k = 3.14159265359 / (double)(2*SINE_BUFFERSIZE);
  double a = (double) SINE_AMPLITUDE;

  for(int i=0; i<SINE_BUFFERSIZE; ++i) {
    sine_buf[i] = a * sin(k * (double)i);
  }
}

sample_t sine(phase_t theta) {
  int p = (theta >> PHASE_SINE_SHIFT);
  int q = p >> SINE_BUFBITS;
  int r = p & (SINE_BUFFERSIZE - 1);

  int i = (q & 1) ? SINE_BUFFERSIZE - r - 1 : r;
  sample_t s = sine_buf[i];

  return (q & 2) ? -s : s;
}

void frequency_setup() {
  double phase;
  double k = 128.0 / 12.0 / (double)FREQ_BUFFERSIZE;
  double f0 = (double)FREQ_STANDARD / exp2((double)NOTE_STANDARD / (double)12) * HERTZ;

  for(int i=0; i<FREQ_BUFFERSIZE; ++i) {
    freq_buf[i] = f0 * exp2(k * (double)i);
  }
}

frequency_t frequency(note_t note) {
  int n = note >> (NOTE_FRACTION - FREQ_FRACTIONBITS);
  return freq_buf[n];
}

void fmfunc_setup() {
  sine_setup();
  frequency_setup();
}
