// fmfunc.h
// FM Maths Functions

#include "stddef.h"
#include "stdint.h"

#define RATE 48000

typedef uint32_t phase_t;
#define PHASE_BITS 32

typedef int32_t frequency_t;
#define HERTZ ((float)UINT32_MAX / (float)RATE)

typedef int32_t sample_t;
#define SAMPLE_FRACTION 24
#define SAMPLE_1 (1<<SAMPLE_FRACTION)

/* Fixed point multiply */
/* May be faster as a 64-bit mul */
#define MUL_INSHIFT (SAMPLE_FRACTION/2)
#define MUL(s1, s2) ((s1>>MUL_INSHIFT) * (s2>>MUL_INSHIFT))

#define SINE_BUFBITS 15
#define PHASE_SIG_BITS (SINE_BUFBITS + 2)
#define PHASE_SINE_SHIFT (PHASE_BITS - PHASE_SIG_BITS)

#define SINE_BUFFERSIZE (1 << SINE_BUFBITS)
#define SINE_AMPLITUDE  SAMPLE_1

typedef uint32_t note_t;
#define NOTE_FRACTION 24
#define NOTE_1 (1<<SAMPLE_FRACTION)

#define NOTE_STANDARD 69
#define FREQ_STANDARD 440.0

#define FREQ_FRACTIONBITS 8
#define FREQ_BUFFERSIZE (128 << FREQ_FRACTIONBITS)

void fmfunc_setup();
sample_t sine(phase_t theta);
frequency_t frequency(note_t note);
