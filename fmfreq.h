// fmosc.h
// FM Oscillator

#include "fmcore.h"
#include "fmosc.h"
#include "stddef.h"

#define NOTE_FRACTION 24
#define NOTE_1 (1<<SAMPLE_FRACTION)

#define NOTE_STANDARD 69
#define FREQ_STANDARD 440.0

#define FREQ_FRACTIONBITS 8
#define FREQ_BUFFERSIZE (128 << FREQ_FRACTIONBITS)

typedef uint32_t note_t;

typedef struct fmfreq_s {
  fmel_t el;
  note_t note;
} fmfreq_t;

void fmfreq_setup();
void fmfreq_init(fmfreq_t *fq);
